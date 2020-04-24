#pragma once

#include "entities.h"
#include <unordered_map>

/*
    NOTE(insolence): Now we only have components struct, but probably make
    some kind of a system
*/


//#define component_map(type)  std::unordered_map<int, type>
//component_map(transform) Transforms;

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
AddResource(int EntityId, components *Components, transform Transform, std::string Texture, resource_type Type, int Amount);

entity
AddUnit(int EntityId, components *Components, transform Transform, motion Motion,
        std::string Texture, unit_type UnitType, int Health, int Power);

entity
AddBuilding(int EntityId, components *Components, transform Transform, std::string Texture, building_type Type, float ProductionTime);

void
StartUnitProduction(std::unordered_map<int, building> &Buildings, std::unordered_map<int, targetable> Targetables);

void
StopUnitProduction(std::unordered_map<int, building> &Buildings);

void
UpdateCollider(transform Transform, collider &Collider);

bool
CurrentCollides(int CurrentIndex, std::unordered_map<int, collider> Colliders);

void
MoveEntities(std::unordered_map<int, transform> &Transforms,
             std::unordered_map<int, motion>    &Motions,
             std::unordered_map<int, collider>  &Colliders);

bool
UpdateBuildingsProduction(std::unordered_map<int, building> &Buildings,
                          float DeltaTime);

int
GetTargetedIndex(std::unordered_map<int, targetable> Targetables);

void
SetUntargetableAll(std::unordered_map<int, targetable> &Targetables);

void
TryToSetTargetable(std::unordered_map<int, targetable> &Targetables,
                   std::unordered_map<int, collider> Colliders, vec2 WorldCursorPos);

void
ChangeTargetedDest(std::unordered_map<int, targetable> Targetables, std::unordered_map<int, motion> &Motions, std::unordered_map<int, unit> Units,
                   std::unordered_map<int, transform> Transforms, std::unordered_map<int, collider> Colliders, vec2 WorldCursorPos);

void
DisplayDrawables(std::unordered_map<int, transform> Transforms,
                 std::unordered_map<int, drawable> Drawables,
                 std::unordered_map<int, targetable> Targetables,
                 std::vector<dir_light> DirLights, std::vector<point_light> PointLights, std::vector<spotlight_light> SpotLights,
                 orthographic_camera *Camera);

void
DisplayAttackerPowers(std::unordered_map<int, transform> Transforms, std::unordered_map<int, attacker> Attackers,
                      orthographic_camera *Camera, color Color);

void
DisplayProductionTime(std::unordered_map<int, transform> Transforms, std::unordered_map<int, building> Buildings,
                      orthographic_camera *Camera);



