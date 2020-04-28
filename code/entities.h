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
    vec2 TargetPos; // NOTE(insolence): if TargetPos == vec2{-1, -1}, then no TargetPos
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

enum resource_type
{
    stone, sapphires
};

struct resource
{
    resource_type Type;
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

struct patrol
{
    vec2 InitialPos;
    float Radius;
    int ChasingEntity;
};

// probably make an enemy component


