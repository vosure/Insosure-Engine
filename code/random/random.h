#pragma once

#include "../math/math.h"

inline float 
GetRandom(void) //NOTE(vosure) XorShift Algorithm
{

    static unsigned int x = 123456789;
    static unsigned int y = 362436069;
    static unsigned int z = 521288629;
    static unsigned int w = 88675123;
    unsigned int t;
    t = x ^ (x << 11);   
    x = y; 
    y = z; 
    z = w;
    w = w ^ (w >> 19) ^ (t ^ (t >> 8));
    return (float)w /  0xFFFFFFFF;
}

inline int
GetRandomInt(int min, int max)
{
    int Value = (TruncateFloatToInt(GetRandom()));
    Value = min + Value * (max - min);

    return Value;
}

inline float
GetRandomFloat(float min, float max)
{
    float Value = GetRandom();
    Value = min + Value * (max - min);

    return Value;
}