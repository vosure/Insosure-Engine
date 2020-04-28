#pragma once

#include <stdio.h>

//////////////////////////////////////////
// NOTE(insolence): Physics for 2D objects
//////////////////////////////////////////

// TODO(insolence): Add Ray-AABB and Ray-Sphere interscetion

// NOTE(insolence): Point == vec2

struct aabb
{
    vec2 Min;
    vec2 Max;
};

struct bounding_circle
{
    point Center;
    float Radius;
};

// FIXME(insolence): This 0.1f is probably wrong
// void
// UpdateAABB(aabb *Box, vec2 NewPos, float Size)
// {
//     Box->Min = NewPos + vec2{0.1f, 0.1f};
//     Box->Max.X = NewPos.X + Size - 0.1f;
//     Box->Max.Y = NewPos.Y + Size - 0.1f;
// }

void
UpdateAABB(aabb *Box, vec2 NewPos, float Size)
{
    Box->Min = NewPos;
    Box->Max.X = NewPos.X + Size;
    Box->Max.Y = NewPos.Y + Size;
}


bool
IsPointInsideAABB(point Point, aabb Box)
{
    return (Point.X <= Box.Max.X && Point.Y <= Box.Max.Y &&
            Point.X >= Box.Min.X && Point.Y >= Box.Min.Y);
}

bool
IsPointInsideCircle(point Point, bounding_circle Circle)
{
    vec2 Distance = Abs(Circle.Center - Point);
    float Dist = Length(Distance);

    return (Dist < Circle.Radius);
}


bool
Intersect(aabb Left, aabb Right)
{
    return (Left.Min.X <= Right.Max.X && Left.Max.X >= Right.Min.X &&
            Left.Min.Y <= Right.Max.Y && Left.Max.Y >= Right.Min.Y);
}

bool
Intersect(bounding_circle Circle, aabb Box)
{
    point BoxHalfExtents;
    BoxHalfExtents.X = Box.Max.X - Box.Min.X;
    BoxHalfExtents.Y = Box.Max.Y - Box.Min.Y;

    point BoxCenter;
    BoxCenter.X = Box.Min.X + BoxHalfExtents.X;
    BoxCenter.Y = Box.Min.Y + BoxHalfExtents.Y;

    vec2 Difference = Circle.Center - BoxCenter;
    vec2 Clamped = Clamp(Difference, -BoxHalfExtents, BoxHalfExtents);

    vec2 Closest = BoxCenter + Clamped;
    Difference = Closest - Circle.Center;

    return (Length(Difference) < Circle.Radius);
}

bool
Intersect(bounding_circle Left, bounding_circle Right)
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
    Left.Min.X = 10;
    Left.Min.Y = 10;
    Left.Max.X = 20;
    Left.Max.Y = 20;

    aabb Right = {};
    Right.Min.X = 10;
    Right.Min.Y = 10;
    Right.Max.X = 20;
    Right.Max.Y = 20;

    if (!Intersect(Left, Right))
        printf("Don't intersect");
    else
        printf("Intersect");
}


//////////////////////////////////////////
// TODO(insolence): Physics for 3D objects
//////////////////////////////////////////

typedef vec3 point3D;

struct aabb_3D
{
    point3D Min;
    point3D Max;
};

struct bounding_sphere
{
    point3D Center;
    float Radius;
};

bool
IsPointInsideAABB(point3D Point, aabb_3D Box)
{
    return (Point.X <= Box.Max.X && Point.Y <= Box.Max.Y && Point.Z <= Box.Max.Z &&
            Point.X >= Box.Min.X && Point.Y >= Box.Min.Y && Point.Z >= Box.Min.Z);
}

bool
IsPointInsideSphere(point3D Point, bounding_sphere Sphere)
{
    vec3 Distance = Abs(Sphere.Center - Point);
    float Dist = Length(Distance);

    return (Dist < Sphere.Radius);
}


bool
Intersect(aabb_3D Left, aabb_3D Right)
{
    return (Left.Min.X <= Right.Max.X && Left.Max.X >= Right.Min.X &&
            Left.Min.Z <= Right.Max.Z && Left.Max.Z >= Right.Min.Z);
}

bool
Intersect(bounding_sphere Sphere, aabb_3D Box)
{
    point3D BoxHalfExtents;
    BoxHalfExtents.X = Box.Max.X - Box.Min.X;
    BoxHalfExtents.Y = Box.Max.Y - Box.Min.Y;
    BoxHalfExtents.Z = Box.Max.Z - Box.Min.Z;

    point3D BoxCenter;
    BoxCenter.X = Box.Min.X + BoxHalfExtents.X;
    BoxCenter.Y = Box.Min.Y + BoxHalfExtents.Y;
    BoxCenter.Z = Box.Min.Z + BoxHalfExtents.Z;

    vec3 Difference = Sphere.Center - BoxCenter;
    vec3 Clamped = Clamp(Difference, -BoxHalfExtents, BoxHalfExtents);

    vec3 Closest = BoxCenter + Clamped;
    Difference = Closest - Sphere.Center;

    return (Length(Difference) < Sphere.Radius);
}

bool
Intersect(bounding_sphere Left, bounding_sphere Right)
{
    vec3 Distance = Abs(Right.Center - Left.Center);
    float Dist = Length(Distance);

    return (Dist < (Left.Radius + Right.Radius));
}
