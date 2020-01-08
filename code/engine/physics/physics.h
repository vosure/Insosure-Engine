#pragma once

#include "../math/math.h"
#include <stdio.h>

typedef vec2 point;

//////////////////////////////////////////////////////////////
// TODO(insolence): Move to math
vec2 Abs(point Vector)
{
    (Vector.X < 0) ? Vector.X = -Vector.X : Vector.X = Vector.X;
    (Vector.Y < 0) ? Vector.Y = -Vector.Y : Vector.Y = Vector.Y;
    
    return Vector;
}
float Max(float Left, float Right)
{
    return (Left > Right ? Left : Right);
}
float Min(float Left, float Right)
{
    return (Left < Right ? Left : Right);
}
float Clamp(float value, float min, float max)
{
    return Max(min, Min(max, value));
}
vec2 Clamp(vec2 value, vec2 min, vec2 max)
{
    vec2 Result;

    Result.X = Max(min.X, Min(max.X, value.X));
    Result.Y = Max(min.Y, Min(max.Y, value.Y));

    return Result;
}
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////
// NOTE(insolence): Physics for 2D objects
//////////////////////////////////////////

struct aabb
{
    int MinX, MinY;
    int MaxX, MaxY;
    vec2 Velocity;
};

struct bounding_sphere
{
    point Center;
    float Radius;
    vec2 Velocity;
};

bool 
IsPointInsideAABB(point Point, aabb Box)
{
    return (Point.X <= Box.MaxX && Point.Y <= Box.MaxY &&
            Point.X >= Box.MinX && Point.X >= Box.MinY);
}

bool
IsPointInsideSphere(point Point, bounding_sphere Sphere)
{
    vec2 Distance = Abs(Sphere.Center - Point);
    float Dist = Length(Distance);

    return (Dist < Sphere.Radius);
}


bool
Intersect(aabb Left, aabb Right)
{
    return (Left.MinX <= Right.MaxX && Left.MaxX >= Right.MinX &&
            Left.MinY <= Right.MaxY && Left.MaxY >= Right.MinY);
}

bool
Intersect(bounding_sphere Sphere, aabb Box)
{
    point BoxHalfExtents;
    BoxHalfExtents.X = Box.MaxX - Box.MinX;
    BoxHalfExtents.Y = Box.MaxY - Box.MinY;

    point BoxCenter;
    BoxCenter.X = Box.MinX + BoxHalfExtents.X;
    BoxCenter.Y = Box.MinY + BoxHalfExtents.Y;

    vec2 Difference = Sphere.Center - BoxCenter;
    vec2 Clamped = Clamp(Difference, -BoxHalfExtents, BoxHalfExtents);

    vec2 Closest = BoxCenter + Clamped;
    Difference = Closest - Sphere.Center;

    return (Length(Difference) < Sphere.Radius);
}

bool
Intersect(bounding_sphere Left, bounding_sphere Right)
{
    vec2 Distance = Abs(Right.Center - Left.Center);
    float Dist = Length(Distance);

    return (Dist < (Left.Radius + Right.Radius));
}

// NOTE(insolence): Test code for collisions
void
TestCollisions()
{
    aabb Left = {};
    Left.MinX = 10;
    Left.MinY = 10;
    Left.MaxX = 20;
    Left.MaxY = 20;

    aabb Right = {};
    Right.MinX = 10;
    Right.MinY = 10;
    Right.MaxX = 20;
    Right.MaxY = 20;

    if (!Intersect(Left, Right))
        printf("Don't intersect");
    else
        printf("Intersect");
}


//////////////////////////////////////////
// TODO(insolence): Physics for 3D objects 
//////////////////////////////////////////



