  
#include <stdio.h>
#include <windows.h>

typedef int (*Start)();

int main()
{
    HINSTANCE dll = LoadLibraryA("../build/Engine.dll");
    if (!dll)
    {
        printf ("Can't load a DLL!");
        return -1;
    }

    Start EngineStartFunc = (Start)GetProcAddress(dll, "Start");
    if (!EngineStartFunc)
    {
        printf("Failed to load a func from DLL!");
        return -1;
    }

    EngineStartFunc();

    return 0;
}