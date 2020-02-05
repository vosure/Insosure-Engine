#pragma once

#include <time.h>

int
TimeSeconds()
{
    return time(NULL);
}

long
TimeMs()
{
    return time(NULL) * 1000l;
}