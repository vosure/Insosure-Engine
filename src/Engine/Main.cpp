#include "Main.h"

#include <stdio.h>

void __declspec(dllexport) __stdcall Start()
{
    printf("Engine has started");
}