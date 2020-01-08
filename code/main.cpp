#include <stdio.h>
#include <windows.h>

typedef int (*Start)();

int 
main()
{
    HINSTANCE DLL = LoadLibrary("../build/engine.dll");
    if (!DLL)
    {
        printf ("Can't load a DLL!");
        return -1;
    }
    Start EngineStartFunc = (Start)GetProcAddress(DLL, "Start");
    if (!EngineStartFunc)
    {
        printf("Failed to load a func from DLL!");
        return -1;
    }
    EngineStartFunc();

    system("pause");

    return 0;
}