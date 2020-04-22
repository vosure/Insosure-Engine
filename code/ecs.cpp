#include "ecs.h"

entity
AddEnemy(int EntityId, components *Components, transform Transform, motion Motion, std::string Texture, int Power, int Health)
{
    entity NewEntity = entity{ EntityId };

    Components->Transforms[EntityId] = Transform;
    Components->Motions[EntityId] = Motion;
    Components->Drawables[EntityId] = drawable{ Texture };
    Components->Healths[EntityId] = health { Health };
    Components->Attackers[EntityId] = attacker { Power };
    Components->Targetables[EntityId] = targetable{true};

    return NewEntity;
}

entity
AddObstacle(int EntityId, components *Components, transform Transform, std::string Texture)
{
    entity NewEntity = entity{ EntityId };

    Components->Transforms[EntityId] = Transform;
    Components->Drawables[EntityId] = drawable{ Texture };
    Components->Targetables[EntityId] = targetable{true};

    return NewEntity;
}

entity
AddChest(int EntityId, components *Components, transform Transform, std::string Texture, int Value)
{
    entity NewEntity = entity{ EntityId };

    Components->Transforms[EntityId] = Transform;
    Components->Drawables[EntityId] = drawable{ Texture };
    Components->Chests[EntityId] = chest{Value};
    Components->Targetables[EntityId] = targetable{true};

    return NewEntity;
}

entity
AddResource(int EntityId, components *Components, transform Transform, std::string Texture, int Type, int Amount)
{
    entity NewEntity = entity{ EntityId };

    Components->Transforms[EntityId] = Transform;
    Components->Drawables[EntityId] = drawable{ Texture };
    Components->Resources[EntityId] = resource{ Type, Amount };
    Components->Targetables[EntityId] = targetable{true};

    return NewEntity;
}

entity
AddUnit(int EntityId, components *Components, transform Transform, motion Motion,
        std::string Texture, unit_type UnitType, int Health, int Power)
{
    entity NewEntity = entity{ EntityId };

    Components->Transforms[EntityId] = Transform;
    Components->Motions[EntityId] = Motion;
    Components->Drawables[EntityId] = drawable{ Texture };
    Components->Healths[EntityId] =  health { Health };
    Components->Attackers[EntityId] =  attacker { Power };
    Components->Units[EntityId] = unit{UnitType};
    Components->Targetables[EntityId] = targetable{true};

    return NewEntity;
}

entity
AddBuilding(int EntityId, components *Components, transform Transform, std::string Texture, building_type Type, float ProductionTime)
{
    entity NewEntity = entity{ EntityId };

    Components->Transforms[EntityId] = Transform;
    Components->Drawables[EntityId] = drawable{ Texture };

    building BuildingComponent = building { false, ProductionTime, ProductionTime, Type};
    Components->Buildings[EntityId] = BuildingComponent;
    Components->Targetables[EntityId] = targetable{true};

    return NewEntity;
}

void
MoveEntities(std::unordered_map<int, transform> &Transforms,
             std::unordered_map<int, motion>    &Motions,
             std::unordered_map<int, collider>  &Colliders)
{
    for (std::pair<int, motion> Element : Motions)
    {

    }
}

// NOTE(insolence): We update the building production time and if it must produce a unit we return true
bool
UpdateBuildingsProduction(std::unordered_map<int, building> &Buildings,
                          float DeltaTime)
{
    for (std::pair<int , building> Element : Buildings)
    {
        building Building = Element.second;
        if (Building.InProduction)
        {
            Building.ProductionTimeLeft -= DeltaTime;
            if (Building.ProductionTimeLeft <= 0)
            {
                Building.InProduction = false;
                Building.ProductionTimeLeft += Building.ProductionTime;

                Buildings[Element.first] = Building;

                return true;
            }
        }
    }

    return false;
}

void
TryToSetTargetable(std::unordered_map<int, targetable> &Targetables,
                   std::unordered_map<int, collider> Colliders, vec2 WorldCursorPos)
{
    for (std::pair<int, collider> Element : Colliders)
    {
        if (IsPointInsideAABB(WorldCursorPos, Element.second.Collider))
        {
            // if (Targetables.find(Element.first))
            // {
            //    Targetables[Element.first].IsTargeted = true;
            //}
        }
    }
}

void
DisplayDrawables(std::unordered_map<int, transform> Transforms,
                 std::unordered_map<int, drawable> Drawables,
                 std::vector<dir_light> DirLights, std::vector<point_light> PointLights, std::vector<spotlight_light> SpotLights,
                 orthographic_camera *Camera)
{
    for (std::pair<int, drawable> Element : Drawables)
    {
        transform TransformComp = Transforms[Element.first];
        DrawRectangleTextured(Camera, Transform(TransformComp.Pos, 0.f, TransformComp.Size/2.f), GetTexture(Element.second.Texture),
                              GetNormalFromTexture(Element.second.Texture), DirLights, PointLights, SpotLights);
    }
}


