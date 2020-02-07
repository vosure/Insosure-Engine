#pragma once

#include <time.h>

int
TimeSeconds()
{
    return time(NULL);
}

long
TimeMilliseconds()
{
    return time(NULL) * 1000l;
}

long
TimeMicroseconds()
{
    return time(NULL) * 1000 * 1000;
}

long
TimeNanoseconds()
{
    return time(NULL) * 1000 * 1000 * 1000;
}