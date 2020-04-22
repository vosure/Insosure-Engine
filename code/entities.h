#pragma once

#include <string.h>

struct entity
{
    int ID;
};

struct transform
{
    vec2 Pos;
    float Size;
};

struct motion
{
    vec2 TargetPos;
    vec2 Velocity;
    float Speed;
};

struct collider
{
    aabb Collider;
};

struct drawable
{
    std::string Texture;
};

struct resource
{
    int Type;
    int Amount;
};

enum building_type
{
    barracks, base
};

struct building
{
    bool InProduction;
    float ProductionTimeLeft;
    float ProductionTime;
    building_type Type;
};

struct targetable
{
    bool IsTargeted;
};

struct health
{
    int Health;
};

struct attacker
{
    int Power;
};

enum unit_type
{
    WORKER = 0,
    ARCHER = 1,
    MELEE = 2,
};

struct unit
{
    unit_type Type;
};

struct chest
{
    int Value;
};

