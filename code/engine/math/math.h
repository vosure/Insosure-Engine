#pragma once

// TODO(insolence): Delete and replace with intrinsics?
#include <math.h>

#define PI 3.14159265358979323846f
#define DEG2RAD(Deg) ((Deg) / 180.0f * PI)
#define RAD2DEG(Rad) ((Rad) / PI * 180.0f)

inline float
Max(float Left, float Right)
{
    return (Left > Right ? Left : Right);
}

inline float
Min(float Left, float Right)
{
    return (Left < Right ? Left : Right);
}

inline float
Clamp(float value, float min, float max)
{
    return Max(min, Min(max, value));
}

inline float
Abs(float Value)
{
    return (Value < 0 ? -Value : Value);
}

//NOTE(vosure):Sqrt, check it before you use
float
Root(float Value)
{
    float Low = 0;
    float High = Value;
    float Middle;
    for (int Index = 0; Index < 1000; Index++)
    {
        Middle = (Low + High) / 2;
        if (Middle * Middle == Value)
            return Middle;
        if (Middle * Middle > Value)
            High = Middle;
        else
            Low = Middle;
    }
    return Middle;
}

//TODO(vosure): Replace to util_math file
inline float
Square(float A)
{
    return A * A;
}

// NOTE(insolence): Check this
inline float
Power(float Value, int Pow)
{
    float Result = Value;
    for (int i = 0; i < Pow; i++)
        Result = Square(Value);

    return Result;
}

inline int
TruncateFloatToInt(float Value)
{
    return (int)Value;
}

inline int
RoundFloatToInt(float Value)
{
    int Result = (int)roundf(Value);
    return Result;
}

inline unsigned int
RoundFloatToUInt(float Value)
{
    unsigned int Result = (unsigned int)roundf(Value);
    return Result;
}

inline int
FloorFloatToInt(float Value)
{
    int Result = (int)floorf(Value);
    return Result;
}

inline float
Sin(float Angle)
{
    float Result = sinf(Angle);
    return Result;
}

inline float
Cos(float Angle)
{
    float Result = cosf(Angle);
    return Result;
}

inline float
ATan2(float Y, float X)
{
    float Result = atan2f(Y, X);
    return Result;
}

int
IsPrime(const int Num)
{
	if (Num < 2) return -1;
	if (Num < 4) return 1;
	if ((Num % 2) == 0) return 0;

	for (int i = 3; i <= floor(sqrt((double)Num)); i += 2)
	{
		if ((Num % i) == 0)
			return 0;
	}
	return 1;
}

// NOTE(insolence): Returns the next prime after x, or x if x is prime
int
NextPrime(int Num)
{
	while (IsPrime(Num) != 1)
	{
		Num++;
	}
	return Num;
}

unsigned long
Hash(unsigned char *Str)
{
    unsigned long Hash = 5381;
    int Char;

    while (Char = *Str++)
        Hash = ((Hash << 5) + Hash) + Char; /* hash * 33 + c */

    return Hash;
}