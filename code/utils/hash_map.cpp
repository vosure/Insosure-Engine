#include "hash_map.h"

// FIXME(insolence): UPDATE CALC HASH TO USE NOT ONLY STRINGS
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

template <typename K, typename V>
internal hash_node<K, V>*
NewNode(K Key, V Value)
{
	hash_node<K, V>* Node = (hash_node<K, V>*)Malloc(sizeof(hash_node<K, V>));
	Node->Key = Key;
	Node->Value = Value;

	return Node;
}

template <typename K, typename V>
internal void
DeleteNode(hash_node<K, V> *Node)
{
	free(Node);
}

template <typename K, typename V>
void
DeleteHashMap(hash_map<K, V> *HashMap)
{
	for (int i = 0; i < HashMap->Size; i++)
    {
		hash_node<K, V>* Node = HashMap->Nodes[i];
		if (Node != NULL)
			DeleteNode(Node);
	}
	free(HashMap->Nodes);
	free(HashMap);
}

template <typename K, typename V>
void
Insert(hash_map<K, V> *HashMap, K Key, V Value)
{
	// NOTE(insolence): Check if we can extend our table
	const int Load = HashMap->Count * 100 / HashMap->Size;
	if (Load > 70)
		ResizeUp<const char*, uint>(HashMap);

	hash_node<K, V>* Node = NewNode(Key, Value);
	int Index = GetHash("Hello", HashMap->Size, 0); // FIXME(insolence): Fix this
	hash_node<K, V>* CurrentNode = HashMap->Nodes[Index];

	int Attempt = 1;
	while (CurrentNode != NULL)
    {
			if (CurrentNode->Key == Key) {
				DeleteNode(CurrentNode);
				HashMap->Nodes[Index] = Node;
				return;
		}
		Index = GetHash("Hello", HashMap->Size, Attempt);
		CurrentNode = HashMap->Nodes[Index];
		Attempt++;
	}
	HashMap->Nodes[Index] = Node;
	HashMap->Count++;
}

template <typename K, typename V>
V
Get(hash_map<K, V> *HashMap, K Key)
{
	int Index = GetHash("Hello", HashMap->Size, 0);
	hash_node<K, V> *Node = HashMap->Nodes[Index];
	int Attempt = 1;
	while (Node != NULL)
    {
		if (Node->Key == Key)
		{
			return Node->Value;
		}

		Index = GetHash("Hello", HashMap->Size, Attempt);
		Node = HashMap->Nodes[Index];
		Attempt++;
	}

	return NULL; // FIXME(insolence): Return smth else
}

template <typename K, typename V>
void
Remove(hash_map<K, V> *HashMap, K Key)
{
	const int Load = HashMap->Count * 100 / HashMap->Size;
	if (Load < 10)
    {
		ResizeDown(HashMap);
	}

	int Index = GetHash("Hello", HashMap->Size, 0);
	hash_node<K, V> *Node = HashMap->Nodes[Index];

	int Attempt = 0;
	while (Node != NULL)
    {
		if (Node->Key == Key)
		{
			DeleteNode(Node);
			HashMap->Nodes[Index] = NULL;
		}

		Index = GetHash("Hello", HashMap->Size, Attempt);
		Node = HashMap->Nodes[Index];
		Attempt++;
	}

	HashMap->Count--;
}

template <typename K, typename V>
hash_map<K, V>*
CreateMapSized(const int BaseSize)
{
	hash_map<K, V>* HashMap = (hash_map<K, V>*)Malloc(sizeof(hash_map<K, V>));
	HashMap->BaseSize = BaseSize;

	HashMap->Size = NextPrime(HashMap->BaseSize);

	HashMap->Count = 0;
	HashMap->Nodes = (hash_node<K, V>**)Calloc(HashMap->Size, sizeof(hash_node<K, V>*));
	return HashMap;
}

template <typename K, typename V>
hash_map<K, V>*
CreateHashMap()
{
	return CreateMapSized<K, V>(BASE_SIZE);
}

template <typename K, typename V>
void
ResizeMap(hash_map<K, V> *HashMap, const int BaseSize)
{
	if (BaseSize < BASE_SIZE)
	{
		return;
	}
	hash_map<K, V> *NewHashMap = CreateMapSized<K, V>(BaseSize);
	for (int i = 0; i < HashMap->Size; i++)
    {
		hash_node<K, V>* Node = HashMap->Nodes[i];
		if (Node != NULL)
        {
			Insert(NewHashMap, Node->Key, Node->Value);
		}
	}

	HashMap->BaseSize = NewHashMap->BaseSize;
	HashMap->Count = NewHashMap->Count;

	const int TempSize = HashMap->Size;
	HashMap->Size = NewHashMap->Size;
	NewHashMap->Size = TempSize;

	hash_node<K, V> **TempNodes = HashMap->Nodes;
	HashMap->Nodes = NewHashMap->Nodes;
	NewHashMap->Nodes = TempNodes;

	DeleteHashMap(NewHashMap);
}

template <typename K, typename V>
void
ResizeUp(hash_map<K, V> *HashMap)
{
	const int NewSize = HashMap->BaseSize * 2;
	ResizeMap<K, V>(HashMap, NewSize);
}

template <typename K, typename V>
internal void
ResizeDown(hash_map<K, V> *HashMap)
{
	const int NewSize = HashMap->BaseSize / 2;
	ResizeMap(HashMap, NewSize);
}