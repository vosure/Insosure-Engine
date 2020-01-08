#pragma once

//TODO(vosure):
//vec2/vec3/vec4 - dot,cross,normalize,length, lerp(+, -. *, /, +=, -=),
//mat3/mat4 - identity, rotation, scale, ortho, transform

union vec2 {
    struct
    {
        float X, Y;
    };
    float E[2];
};

inline vec2
operator+(vec2 A, vec2 B)
{
    A.X = A.X + B.X;
    A.Y = A.Y + B.Y;
    return A;
}

inline vec2
operator-(vec2 A, vec2 B)
{
    A.X = A.X - B.X;
    A.Y = A.Y - B.Y;
    return A;
}

inline vec2
operator*(vec2 A, float B)
{
    A.X = A.X * B;
    A.Y = A.Y * B;
    return A;
}

inline vec2
operator*(float B, vec2 A)
{
    A = A * B;
    return A;
}

inline vec2 &
operator+=(vec2 &A, vec2 B)
{
    A = A + B;
    return A;
}

inline vec2 &
operator-=(vec2 &A, vec2 B)
{
    A = A - B;
    return (A);
}

inline vec2 &
operator*=(vec2 &A, float B)
{
    A = A * B;
    return (A);
}

inline vec2
operator-(vec2 A)
{
    A.X = -A.X;
    A.Y = -A.Y;
    return (A);
}

inline float
Dot(vec2 A, vec2 B)
{
    float Result = A.X * B.X + A.Y * B.Y;
    return (Result);
}

//TODO(vosure): SquareRoot function

//NOTE(vosure):Sqrt, check it before you use
float Root(float Value)
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

inline float
Length(vec2 A)
{
    return Root(Square(A.X) + Square(A.Y));
}

inline vec2
Normalize(vec2 A)
{
    return (A * (1.0f / Length(A)));
}

inline vec2
Lerp(vec2 A, vec2 B, float t)
{
    return (A + (B - A) * t);
}

//TODO(vosure):Custom String
inline
void PrintVec2(vec2 Vector)
{
    printf("Vec2: X = %.2f, Y = %.2f", Vector.X, Vector.Y);
}
