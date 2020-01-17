#include "hash_map.h"

global_variable const int PRIME_1 = 151;
global_variable const int PRIME_2 = 163;
internal int
CalcHash(const char *Str, int Prime, int Buckets)
{
	long Hash = 0;
	int StrLength = strlen(Str);
	for (int i = 0; i < StrLength; i++)
    {
		Hash += (long)pow(Prime, StrLength - (i + 1)) * Str[i];
		Hash = Hash % Buckets;
	}
	return (int)Hash;
}

// NOTE(insolence): Double hashing to prevent collisions
internal int
GetHash(const char *Str, int NumBuckets, int Attempt)
{
	int HashA = CalcHash(Str, PRIME_1, NumBuckets);
	int HashB = CalcHash(Str, PRIME_2, NumBuckets);
	return (HashA + (Attempt * (HashB + 1))) % NumBuckets;
}

internal hash_node*
NewNode(const char* k, int v)
{
	hash_node* Node = (hash_node*)malloc(sizeof(hash_node));
	Node->Key = strdup(k);
	Node->Value = v;
	return Node;
}

internal void
DeleteNode(hash_node *Node)
{
	free(Node->Key);
	free(Node);
}

void
DeleteHashMap(hash_map *HashMap)
{
	for (int i = 0; i < HashMap->Size; i++)
    {
		hash_node* Node = HashMap->Nodes[i];
		if (Node != NULL)
			DeleteNode(Node);
	}
	free(HashMap->Nodes);
	free(HashMap);
}

void
Insert(hash_map *HashMap, const char *Key, int Value)
{
	// NOTE(insolence): Check if we can extend our table
	const int Load = HashMap->Count * 100 / HashMap->Size;
	if (Load > 70)
		ResizeUp(HashMap);

	hash_node* Node = NewNode(Key, Value);
	int Index = GetHash(Node->Key, HashMap->Size, 0);
	hash_node* CurrentNode = HashMap->Nodes[Index];

	int Attempt = 1;
	while (CurrentNode != NULL)
    {
		if (CurrentNode != &REMOVED_NODE)
        {
			if (strcmp(CurrentNode->Key, Key) == 0) {
				DeleteNode(CurrentNode);
				HashMap->Nodes[Index] = Node;
				return;
			}
		}
		Index = GetHash(Node->Key, HashMap->Size, Attempt);
		CurrentNode = HashMap->Nodes[Index];
		Attempt++;
	}
	HashMap->Nodes[Index] = Node;
	HashMap->Count++;
}

int
Get(hash_map *HashMap, const char *Key)
{
	int Index = GetHash(Key, HashMap->Size, 0);
	hash_node *Node = HashMap->Nodes[Index];
	int Attempt = 1;
	while (Node != NULL)
    {
		if (Node != &REMOVED_NODE)
        {
			if (strcmp(Node->Key, Key) == 0)
            {
				return Node->Value;
			}
		}
		Index = GetHash(Key, HashMap->Size, Attempt);
		Node = HashMap->Nodes[Index];
		Attempt++;
	}
	return -1;
}

void
Remove(hash_map *HashMap, const char *Key)
{
	const int Load = HashMap->Count * 100 / HashMap->Size;
	if (Load < 10)
    {
		ResizeDown(HashMap);
	}

	int Index = GetHash(Key, HashMap->Size, 0);
	hash_node *Node = HashMap->Nodes[Index];

	int Attempt = 0;
	while (Node != NULL)
    {
		if (Node != &REMOVED_NODE)
        {
			if (strcmp(Node->Key, Key) == 0)
            {
				DeleteNode(Node);
				HashMap->Nodes[Index] = &REMOVED_NODE;
			}
		}
		Index = GetHash(Key, HashMap->Size, Attempt);
		Node = HashMap->Nodes[Index];
		Attempt++;
	}

	HashMap->Count--;
}

internal hash_map*
CreateMapSized(const int BaseSize)
{
	hash_map* HashMap = (hash_map*)malloc(sizeof(hash_map));
	HashMap->BaseSize = BaseSize;

	HashMap->Size = NextPrime(HashMap->BaseSize);

	HashMap->Count = 0;
	HashMap->Nodes = (hash_node**)calloc(HashMap->Size, sizeof(hash_node*));
	return HashMap;
}

hash_map*
CreateMap()
{
	return CreateMapSized(BASE_SIZE);
}

internal void
ResizeMap(hash_map *HashMap, const int BaseSize) {
	if (BaseSize < BASE_SIZE) {
		return;
	}
	hash_map *NewHashMap = CreateMapSized(BaseSize);
	for (int i = 0; i < HashMap->Size; i++)
    {
		hash_node* Node = HashMap->Nodes[i];
		if (Node != NULL && Node != &REMOVED_NODE)
        {
			Insert(NewHashMap, Node->Key, Node->Value);
		}
	}

	HashMap->BaseSize = NewHashMap->BaseSize;
	HashMap->Count = NewHashMap->Count;

	const int TempSize = HashMap->Size;
	HashMap->Size = NewHashMap->Size;
	NewHashMap->Size = TempSize;

	hash_node **TempNodes = HashMap->Nodes;
	HashMap->Nodes = NewHashMap->Nodes;
	NewHashMap->Nodes = TempNodes;

	DeleteHashMap(NewHashMap);
}

internal void
ResizeUp(hash_map *HashMap)
{
	const int NewSize = HashMap->BaseSize * 2;
	ResizeMap(HashMap, NewSize);
}

internal void
ResizeDown(hash_map *HashMap)
{
	const int NewSize = HashMap->BaseSize / 2;
	ResizeMap(HashMap, NewSize);
}