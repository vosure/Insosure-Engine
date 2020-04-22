#pragma once

#include "entities.h"
#include <unordered_map>

/*
    NOTE(insolence): Now we only have components struct, but probably make
    some kind of a system
*/


//#define component_map<type> std::unordered_map<int, type>

struct components
{
    // component_map<transform> Transforms;
    std::unordered_map<int, transform> Transforms;
    std::unordered_map<int, motion> Motions;
    std::unordered_map<int, collider> Colliders;
    std::unordered_map<int, drawable> Drawables;
    std::unordered_map<int, resource> Resources;
    std::unordered_map<int, building> Buildings;
    std::unordered_map<int, targetable> Targetables;
    std::unordered_map<int, health> Healths;
    std::unordered_map<int, attacker> Attackers;
    std::unordered_map<int, unit> Units;
    std::unordered_map<int, chest> Chests;
};

// NOTE(insolence): Enemies, units and buildings are targetable by default
entity
AddEnemy(int EntityId, components *Components, transform Transform, motion Motion, std::string Texture, int Power, int Health);

entity
AddObstacle(int EntityId, components *Components, transform Transform, std::string Texture);

entity
AddChest(int EntityId, components *Components, transform Transform, std::string Texture, int Value);

entity
AddResource(int EntityId, components *Components, transform Transform, std::string Texture, int Type, int Amount);

entity
AddUnit(int EntityId, components *Components, transform Transform, motion Motion,
        std::string Texture, unit_type UnitType, int Health, int Power);

entity
AddBuilding(int EntityId, components *Components, transform Transform, std::string Texture, building_type Type, float ProductionTime);

void
MoveEntities(std::unordered_map<int, transform> &Transforms,
             std::unordered_map<int, motion>    &Motions,
             std::unordered_map<int, collider>  &Colliders);

bool
UpdateBuildingsProduction(std::unordered_map<int, building> &Buildings,
                          float DeltaTime);

void
TryToSetTargetable(std::unordered_map<int, targetable> &Targetables,
                   std::unordered_map<int, collider> Colliders, vec2 WorldCursorPos);

void
DisplayDrawables(std::unordered_map<int, transform> Transforms,
                 std::unordered_map<int, drawable> Drawables,
                 std::vector<dir_light> DirLights, std::vector<point_light> PointLights, std::vector<spotlight_light> SpotLights,
                 orthographic_camera *Camera);


