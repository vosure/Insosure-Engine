#pragma once

#include <stdio.h>

// TODO(insolence): Probably use templates or macros, as we
// may need to store different types

template <class T>
struct array_list
{
    T **Elements; // ints for now
    int CurrentSize;
    int Capacity;
    // float LoadFactor = 0.75f;
};


template<typename T>
array_list<T>*
CreateList(int InitialCapacity = 20)
{
    array_list<T> *Result = (array_list<T>*)Malloc(sizeof(array_list<T>));
    Result->Capacity = InitialCapacity;
    Result->CurrentSize = 0;
    Result->Elements = (T**)Calloc(InitialCapacity, sizeof(T));

    return Result;
}

template<typename T>
void
DestroyList(array_list<T> *List)
{
    free(List);
}

template<typename T>
void
PushToList(array_list<T> *List, T *Value)
{
    if (List->CurrentSize < List->Capacity)
    {
        List->Elements[List->CurrentSize] = Value;
        List->CurrentSize++;
    }
    else
    {
        // TODO(insolence): Resize
        Resize(List, List->Capacity * 2);
        PushToList(List, Value);
    }
}

template<typename T>
void
AddIndexedToList(array_list<T> *List, int Index, T *Value)
{

    if (Index >= 0 && Index < List->Capacity)
    {
        List->Elements[Index] = Value;
        List->CurrentSize++;
    }
}

template<typename T>
void
RemoveFromList(array_list<T> *List, int Index)
{
    if (Index >= 0 && Index < List->Capacity)
    {
        List->Elements[Index] = NULL; // FIXME(insolence): Temp solution
    }
}

template<typename T>
void
Clear(array_list<T> *List)
{
    for (int i = 0; i < List->Capacity; i++)
    {
        List->Elements[i] = NULL;
    }
}

template<typename T>
T*
Get(array_list<T> *List, int Index)
{
    if (Index >= 0 && Index < List->Capacity)
    {
        return List->Elements[Index];
    }
    else
        return NULL;
}

template<typename T>
bool
ListContains(array_list<T> *List, T Value)
{
    for (int i = 0; i < List->Capacity; i++)
    {
        if (List->Elements[i] == &Value)
            return true;
    }

    return false;
}


// NOTE(insolence): Currently resizing up
template<typename T>
void
Resize(array_list<T> *List, int NewCapacity)
{
    if (NewCapacity > List->Capacity)
    {
        List->Elements = (T**)realloc(List->Elements, sizeof(T) * NewCapacity);
    }
}