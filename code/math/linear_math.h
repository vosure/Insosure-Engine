#pragma once

#include "math.h"
//TODO(vosure):
//vec2/vec3/vec4 - dot,cross,normalize,length, lerp(+, -. *, /, +=, -=),
//mat3/mat4 - identity, rotation, scale, ortho, transform

//TODO(vosure) Hadamard product, Clamp, Max, Min, (>, <)
//TODO(vosure) SIMD Math
union vec2 {
    struct
    {
        float X, Y;
    };
    float E[2];
};
typedef vec2 point;


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

    return A;
}

inline vec2 &
operator*=(vec2 &A, float B)
{
    A = A * B;

    return A;
}

inline vec2
operator-(vec2 A)
{
    A.X = -A.X;
    A.Y = -A.Y;

    return A;
}

inline bool
operator==(vec2 A, vec2 B)
{
    return (A.X == B.X && A.Y == B.Y);
}

inline bool
operator!=(vec2 A, vec2 B)
{
    return (A.X != B.X && A.Y != B.Y);
}

inline float
Dot(vec2 A, vec2 B)
{
    float Result = A.X * B.X + A.Y * B.Y;

    return Result;
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

inline vec2
Abs(vec2 Vector)
{
    (Vector.X < 0) ? Vector.X = -Vector.X : Vector.X = Vector.X;
    (Vector.Y < 0) ? Vector.Y = -Vector.Y : Vector.Y = Vector.Y;

    return Vector;
}

inline vec2
Clamp(vec2 value, vec2 min, vec2 max)
{
    vec2 Result;

    Result.X = Max(min.X, Min(max.X, value.X));
    Result.Y = Max(min.Y, Min(max.Y, value.Y));

    return Result;
}

inline void
PrintVec2(vec2 Vector)
{
    printf("Vec2: X = %.2f, Y = %.2f \n", Vector.X, Vector.Y);
}

//NOTE(vosure): vec3
union vec3 {
    struct
    {
        float X, Y, Z;
    };
    struct
    {
        float R, G, B;
    };
    float E[3];
};
typedef vec3 color;

inline vec3
operator+(vec3 A, vec3 B)
{
    A.X = A.X + B.X;
    A.Y = A.Y + B.Y;
    A.Z = A.Z + B.Z;

    return A;
}

inline vec3
operator-(vec3 A, vec3 B)
{
    A.X = A.X - B.X;
    A.Y = A.Y - B.Y;
    A.Z = A.Z - B.Z;

    return A;
}

inline vec3
operator*(vec3 A, float B)
{
    A.X = A.X * B;
    A.Y = A.Y * B;
    A.Z = A.Z * B;

    return A;
}

inline vec3
operator*(float B, vec3 A)
{
    A = A * B;

    return A;
}

inline vec3 &
operator+=(vec3 &A, vec3 B)
{
    A = A + B;

    return A;
}

inline vec3 &
operator-=(vec3 &A, vec3 B)
{
    A = A - B;

    return A;
}

inline vec3 &
operator*=(vec3 &A, float B)
{
    A = A * B;

    return A;
}

inline vec3
operator-(vec3 A)
{
    A.X = -A.X;
    A.Y = -A.Y;
    A.Z = -A.Z;

    return A;
}

inline float
Dot(vec3 A, vec3 B)
{
    float Result = A.X * B.X + A.Y * B.Y + A.Z * B.Z;

    return Result;
}

inline vec3
Cross(vec3 A, vec3 B)
{
    A.X = A.Y * B.Z - A.Z * B.Y;
    A.Y = A.Z * B.X - A.X * B.Z;
    A.Z = A.X * B.Y - A.Y * A.X;

    return A;
}

inline float
Length(vec3 A)
{
    return Root(Square(A.X) + Square(A.Y) + Square(A.Z));
}

inline vec3
Normalize(vec3 A)
{
    return (A * (1.0f / Length(A)));
}

inline vec3
Lerp(vec3 A, vec3 B, float t)
{
    return (A + (B - A) * t);
}

inline vec3
Abs(vec3 Vector)
{
    (Vector.X < 0) ? Vector.X = -Vector.X : Vector.X = Vector.X;
    (Vector.Y < 0) ? Vector.Y = -Vector.Y : Vector.Y = Vector.Y;
    (Vector.Z < 0) ? Vector.Z = -Vector.Z : Vector.Z = Vector.Z;

    return Vector;
}

inline vec3
Clamp(vec3 value, vec3 min, vec3 max)
{
    vec3 Result;

    Result.X = Max(min.X, Min(max.X, value.X));
    Result.Y = Max(min.Y, Min(max.Y, value.Y));
    Result.Z = Max(min.Z, Min(max.Z, value.Z));

    return Result;
}

//TODO(vosure):Custom String
inline void PrintVec3(vec3 Vector)
{
    printf("Vec3: X = %.2f, Y = %.2f, Z = %.2f \n", Vector.X, Vector.Y, Vector.Z);
}

//NOTE(vosure): vec4
union vec4 {
    struct
    {
        float X, Y, Z, W;
    };
    struct
    {
        float R, G, B, A;
    };
    float E[4];
};
typedef vec4 color4;

inline vec4
operator+(vec4 A, vec4 B)
{
    A.X = A.X + B.X;
    A.Y = A.Y + B.Y;
    A.Z = A.Z + B.Z;
    A.W = A.W + B.W;

    return A;
}

inline vec4
operator-(vec4 A, vec4 B)
{
    A.X = A.X - B.X;
    A.Y = A.Y - B.Y;
    A.Z = A.Z - B.Z;
    A.W = A.W - B.W;

    return A;
}

inline vec4
operator*(vec4 A, float B)
{
    A.X = A.X * B;
    A.Y = A.Y * B;
    A.Z = A.Z * B;
    A.W = A.W * B;

    return A;
}

inline vec4
operator*(float B, vec4 A)
{
    A = A * B;

    return A;
}

inline vec4 &
operator+=(vec4 &A, vec4 B)
{
    A = A + B;

    return A;
}

inline vec4 &
operator-=(vec4 &A, vec4 B)
{
    A = A - B;

    return A;
}

inline vec4 &
operator*=(vec4 &A, float B)
{
    A = A * B;

    return A;
}

inline vec4
operator-(vec4 A)
{
    A.X = -A.X;
    A.Y = -A.Y;
    A.Z = -A.Z;
    A.W = -A.W;

    return A;
}

inline float
Dot(vec4 A, vec4 B)
{
    float Result = A.X * B.X + A.Y * B.Y + A.Z * B.Z + A.W * B.W;

    return Result;
}

inline float
Length(vec4 A)
{
    return Root(Square(A.X) + Square(A.Y) + Square(A.Z));
}

inline vec4
Normalize(vec4 A)
{
    return (A * (1.0f / Length(A)));
}

inline vec4
Lerp(vec4 A, vec4 B, float t)
{
    return (A + (B - A) * t);
}

//TODO(vosure):Custom String
inline void
PrintVec4(vec4 Vector)
{
    printf("Vec4: X = %.2f, Y = %.2f, Z = %.2f, W = %.2f", Vector.X, Vector.Y, Vector.Z, Vector.W);
}

struct mat4
{
    union {
        struct
        {
            vec4 FirstColumn;
            vec4 SecondColumn;
            vec4 ThirdColumn;
            vec4 FourthColumn;
        };
        float Elements[4 * 4];
    };
};

inline mat4
Identity(float Diagonal = 1.0f)
{
    //NOTE(vosure) Check mat4 Result = {};
    mat4 Result;
    for (int Index = 0; Index < 16; Index++)
    {
        Result.Elements[Index] = 0.0f;
    }
    Result.Elements[0] = Diagonal;
    Result.Elements[5] = Diagonal;
    Result.Elements[10] = Diagonal;
    Result.Elements[15] = Diagonal;

    return Result;
}

// inline mat4
// operator*(mat4 A, mat4 B)
// {
//     float Data[16];
//     for (int X = 0; X < 4; X++)
//     {
//         for (int Y = 0; Y < 4; Y++)
//         {
//             float Result = 0.0f;
//             for (int K = 0; K < 4; K++)
//             {
//                 Result += A.Elements[X * 4 + K] * B.Elements[K * 4 + Y];
//             }
//             Data[X * 4 + Y] = Result;
//         }
//     }

//     memcpy(A.Elements, Data, 16 * sizeof(float));

//     return A;
// }

inline mat4
operator*(mat4 A, mat4 B)
{
    mat4 Result;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            Result.Elements[i * 4 + j] = A.Elements[i * 4 + 0] * B.Elements[0 * 4 + j] +
                                         A.Elements[i * 4 + 1] * B.Elements[1 * 4 + j] +
                                         A.Elements[i * 4 + 2] * B.Elements[2 * 4 + j] +
                                         A.Elements[i * 4 + 3] * B.Elements[3 * 4 + j];
        }
    }
    return Result;
}

inline vec3
operator*(mat4 A, vec3 V)
{
    vec3 Result{
        A.FirstColumn.X * V.X + A.SecondColumn.X * V.Y + A.ThirdColumn.X * V.Z + A.FourthColumn.X,
        A.FirstColumn.Y * V.X + A.SecondColumn.Y * V.Y + A.ThirdColumn.Y * V.Z + A.FourthColumn.Y,
        A.FirstColumn.Z * V.X + A.SecondColumn.Z * V.Y + A.ThirdColumn.Z * V.Z + A.FourthColumn.Z};

    return Result;
}

inline vec4
operator*(mat4 A, vec4 V)
{
    vec4 Result{
        A.FirstColumn.X * V.X + A.SecondColumn.X * V.Y + A.ThirdColumn.X * V.Z + A.FourthColumn.X + V.W,
        A.FirstColumn.Y * V.X + A.SecondColumn.Y * V.Y + A.ThirdColumn.Y * V.Z + A.FourthColumn.Y + V.W,
        A.FirstColumn.Z * V.X + A.SecondColumn.Z * V.Y + A.ThirdColumn.Z * V.Z + A.FourthColumn.Z + V.W,
        A.FirstColumn.W * V.X + A.SecondColumn.W * V.Y + A.ThirdColumn.W * V.Z + A.FourthColumn.W + V.W,
    };

    return Result;
}

inline mat4
Translate(vec3 Translation)
{
    mat4 Result = Identity(1.0f);

    Result.FourthColumn = vec4{Translation.X, Translation.Y, Translation.Z, 1.0f};

    return (Result);
}

inline mat4
Scale(float Scale)
{
    mat4 Result = Identity(1.0f);

    Result.Elements[0] = Scale;
    Result.Elements[5] = Scale;
    Result.Elements[10] = Scale;

    return (Result);
}

inline mat4
Scale(vec3 Scale)
{
    mat4 Result = Identity(1.0f);

    Result.Elements[0] = Scale.X;
    Result.Elements[5] = Scale.Y;
    Result.Elements[10] = Scale.Z;

    return (Result);
}

inline mat4
Rotate(float Angle, vec3 Axis)
{
    mat4 Result = Identity(1.0f);

    float Cosine = cos(DEG2RAD(Angle));
    float Sine = sin(DEG2RAD(Angle));
    float OneMinusCos = (1.0f - Cosine);

    Axis = Normalize(Axis);
    float AxisX = Axis.X;
    float AxisY = Axis.Y;
    float AxisZ = Axis.Z;


    Result.Elements[0 + 0 * 4] = Cosine + (AxisX * AxisX) * OneMinusCos;
    Result.Elements[1 + 0 * 4] = AxisY * AxisX * OneMinusCos + AxisZ * Sine;
    Result.Elements[2 + 0 * 4] = AxisZ * AxisX * OneMinusCos - AxisY * Sine;

    Result.Elements[0 + 1 * 4] = AxisX * AxisY * OneMinusCos - AxisZ * Sine;
    Result.Elements[1 + 1 * 4] = Cosine + (AxisY * AxisY) * OneMinusCos;
    Result.Elements[2 + 1 * 4] = AxisZ * AxisY * OneMinusCos + AxisX * Sine;

    Result.Elements[0 + 2 * 4] = AxisX * AxisZ * OneMinusCos + AxisY * Sine;
    Result.Elements[1 + 2 * 4] = AxisY * AxisZ * OneMinusCos - AxisX * Sine;
    Result.Elements[2 + 2 * 4] = Cosine + (AxisZ * AxisZ) * OneMinusCos;

    return Result;
}

inline mat4
Ortho(float Bot, float Top, float Left, float Right, float Near, float Far)
{
	mat4 Result = Identity(1.0f);

	Result.FirstColumn = vec4{2.0f / (Right - Left), 0.0f, 0.0f, 0.0f};
	Result.SecondColumn = vec4{0.0f, 2.0f / (Top - Bot), 0.0f, 0.0f};
	Result.ThirdColumn = vec4{0.0f, 0.0f, -2.0f / (Far - Near), 0.0f};
	Result.FourthColumn = vec4{-(Right + Left) / (Right - Left), -(Top + Bot) / (Top - Bot),-(Far + Near) / (Far - Near), 1.0f};

	return Result;
}

inline mat4
Perspective(float FoV, float AspectRatio, float Near, float Far)
{
	mat4 Result = Identity(1.0f);

	float Scale = tanf(DEG2RAD(FoV)*0.5f) * Near;
	float Top = Scale;
	float Bottom = -Top;
	float Right = AspectRatio * Top;
	float Left = -Right;


	Result.FirstColumn = vec4{2.0f * Near / (Right - Left), 0.0f, 0.0f, 0.0f};
	Result.SecondColumn = vec4{0.0f, 2.0f * Near / (Top - Bottom), 0.0f, 0.0f};
	Result.ThirdColumn = vec4{(Right + Left) / (Right - Left), (Top + Bottom) / (Top - Bottom), -(Far + Near) / (Far - Near), -1.0f};
	Result.FourthColumn = vec4{0.0f, 0.0f, -(2.0f * Far * Near) / (Far - Near), 0.0f};

	return(Result);
}

inline mat4
LookAt(vec3 From, vec3 Target, vec3 UpAxis = vec3{0.0f, 1.0f, 0.0f})
{
	mat4 Result = Identity(1.0f);

	vec3 Forward = Normalize(From - Target);
	vec3 Right = Normalize(Cross(UpAxis, Forward));
	vec3 Up = Cross(Forward, Right);


	Result.FirstColumn = vec4{Right.X, Up.X, Forward.X, 0.0f};
	Result.SecondColumn = vec4{Right.Y, Up.Y, Forward.Y, 0.0f};
	Result.ThirdColumn = vec4{Right.Z, Up.Z, Forward.Z, 0.0f};
	Result.FourthColumn = vec4{-Dot(From, Right), -Dot(From, Up), -Dot(From, Forward), 1.0f};

	return(Result);
}

// NOTE(insolence): Temp, probably make a separate struct Transform
mat4
Transform(vec2 Pos, float Rotation, float ScaleAm)
{
    mat4 Result = Scale(ScaleAm) * Rotate(Rotation, {0, 0, 1}) * Translate({Pos.X, Pos.Y, 0});

    return Result;
}