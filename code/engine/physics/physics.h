#pragma once

#include <stdio.h>

//////////////////////////////////////////
// NOTE(insolence): Physics for 2D objects
//////////////////////////////////////////

struct aabb
{
    int MinX, MinY;
    int MaxX, MaxY;
    vec2 Velocity;
};

struct bounding_circle
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
IsPointInsideCircle(point Point, bounding_circle Circle)
{
    vec2 Distance = Abs(Circle.Center - Point);
    float Dist = Length(Distance);

    return (Dist < Circle.Radius);
}


bool
Intersect(aabb Left, aabb Right)
{
    return (Left.MinX <= Right.MaxX && Left.MaxX >= Right.MinX &&
            Left.MinY <= Right.MaxY && Left.MaxY >= Right.MinY);
}

bool
Intersect(bounding_circle Circle, aabb Box)
{
    point BoxHalfExtents;
    BoxHalfExtents.X = Box.MaxX - Box.MinX;
    BoxHalfExtents.Y = Box.MaxY - Box.MinY;

    point BoxCenter;
    BoxCenter.X = Box.MinX + BoxHalfExtents.X;
    BoxCenter.Y = Box.MinY + BoxHalfExtents.Y;

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

typedef vec3 point3D;

struct aabb_3D
{
    point3D Min;
    point3D Max;
    vec3 Velocity;
};

struct bounding_sphere
{
    point3D Center;
    float Radius;
    vec3 Velocity;
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
