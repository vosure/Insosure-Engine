#pragma once

// TODO(insolence): Probably use templates or macros, as we
// need to store different types (Shaders, Textures)

struct array_list
{
    int *Elements; // ints for now
    int CurrentSize;
    int Capacity;
    // float LoadFactor = 0.75f;
};
typedef array_list vector;

#define EMPTY -1000000

void CreateList();
void DestroyList(array_list *List);
void AddToList(array_list *List, int Value); // NOTE(insolence): Same as PushBack, add to the end of the list
void AddIndexedToList(array_list *List, int Index, int Value);
void RemoveFromList(array_list *List, int Index);
void Clear(array_list *List);

int Get(array_list *List, int Index);
bool ListContains(array_list *List, int Value);

void Resize(array_list *List, int NewCapacity);

array_list*
CreateList(int InitialCapacity = 20)
{
    array_list *Result = (array_list*)malloc(sizeof(array_list));
    Result->Capacity = InitialCapacity;
    Result->CurrentSize = 0;
    Result->Elements = (int*)calloc(InitialCapacity, sizeof(int)); 

    return Result;
}

void 
DestroyList(array_list *List)
{
    free(List);
}

void
AddToList(array_list *List, int Value)
{
    if (List->CurrentSize < List->Capacity)
    {
        List->Elements[List->CurrentSize - 1] = Value;
        List->CurrentSize++;
    }
    else
    {
        // TODO(insolence): Resize
        Resize(List, List->Capacity * 2);
        AddToList(List, Value);
    }
}

void
AddIndexedToList(array_list *List, int Index, int Value)
{
    if (Index >= 0 && Index < List->Capacity)
    {
        List->Elements[Index] = Value;
    }
}

void 
RemoveFromList(array_list *List, int Index)
{
    if (Index >= 0 && Index < List->Capacity)
    {
        List->Elements[Index] = EMPTY; // FIXME(insolence): Temp solution
    }
}

void 
Clear(array_list *List)
{
    for (int i = 0; i < List->Capacity; i++)
    {
        List->Elements[i] = EMPTY;
    }
}


int
Get(array_list *List, int Index)
{
    if (Index >= 0 && Index < List->Capacity)
    {
        return List->Elements[Index];
    }
    else
        return EMPTY;
}

bool 
ListContains(array_list *List, int Value)
{
    for (int i = 0; i < List->Capacity; i++)
    {
        if (List->Elements[i] == Value)
            return true;
    }

    return false;
}


// NOTE(insolence): Currently resizing up
void 
Resize(array_list *List, int NewCapacity)
{
    if (NewCapacity > List->Capacity)
    {
        List->Elements = (int*)realloc(List->Elements, sizeof(int) * NewCapacity);
    }
}
