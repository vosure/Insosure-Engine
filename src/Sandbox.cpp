#include <windows.h>
#include <stdio.h>

typedef void (__stdcall *Start)();


int main()
{
    HINSTANCE dll = LoadLibraryA("C:/dev/InsosureEngine/src/Engine/Main.cpp");
    if (!dll)
    {
        printf("Something went wrong with the DLL!");
        return 0;
    }

    Start StartupFunc = (Start)GetProcAddress(dll, "Start");
    if (!StartupFunc)
    {
        printf("Something went wrong with the func loading!");
        return 0;
    }


    return 0;
}