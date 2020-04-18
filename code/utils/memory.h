#pragma once

#include <cstdlib>

struct memory_tracker
{
    int BytesInUse;
};

// TODO(insolence): Make this non-static
memory_tracker MemoryTracker;

void*
Malloc(size_t Size)
{
    MemoryTracker.BytesInUse += Size;
    return malloc(Size);
}

void*
Calloc(size_t Num, size_t Size)
{
    MemoryTracker.BytesInUse += Size * Num;
    return calloc(Num, Size);
}

void*
Realloc(void *Ptr, size_t NewSize)
{
    // MemoryTracker.BytesInUse += ???
    return realloc(Ptr, NewSize);
}

void
Free(void *Ptr, size_t Size)
{
    MemoryTracker.BytesInUse -= Size;
    free(Ptr);
}

void
PrintMemoryUsage()
{
    printf("Memory in use: %d \n", MemoryTracker.BytesInUse);
}