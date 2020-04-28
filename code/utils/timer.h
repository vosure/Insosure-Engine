#include <stdio.h>
#include <time.h>

struct timer
{
    clock_t StartTime, EndTime;
    char FuncName[50];
};

void
Start(timer &Timer, const char *Name)
{
    Timer.StartTime = clock();
    strcpy(Timer.FuncName, Name);
}

void
TimePassed(timer &Timer)
{
    Timer.EndTime = clock();
    printf("%s: %dms \n", Timer.FuncName, Timer.EndTime - Timer.StartTime);
}