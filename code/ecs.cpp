#include "ecs.h"

collider MakeCollider(transform Transform)
{
    collider ColliderComponent;
    ColliderComponent.Collider.Min = Transform.Pos + vec2{0.1f, 0.1f};
    ColliderComponent.Collider.Max.X = Transform.Pos.X + Transform.Size - 0.1f;
    ColliderComponent.Collider.Max.Y = Transform.Pos.Y + Transform.Size - 0.1f;

    return ColliderComponent;
}

entity
AddEnemy(int EntityId, components *Components, transform Transform, motion Motion, std::string Texture, int Power, int Health)
{
    entity NewEntity = entity{ EntityId };

    Components->Transforms[EntityId] = Transform;
    Components->Motions[EntityId] = Motion;
    Components->Drawables[EntityId] = drawable{ Texture };
    Components->Healths[EntityId] = health { Health };
    Components->Attackers[EntityId] = attacker { Power };
    Components->Targetables[EntityId] = targetable{false};
    Components->Colliders[EntityId] = MakeCollider(Transform);

    return NewEntity;
}

entity
AddObstacle(int EntityId, components *Components, transform Transform, std::string Texture)
{
    entity NewEntity = entity{ EntityId };

    Components->Transforms[EntityId] = Transform;
    Components->Drawables[EntityId] = drawable{ Texture };
    //Components->Targetables[EntityId] = targetable{false};
    Components->Colliders[EntityId] = MakeCollider(Transform);

    return NewEntity;
}

entity
AddChest(int EntityId, components *Components, transform Transform, std::string Texture, int Value)
{
    entity NewEntity = entity{ EntityId };

    Components->Transforms[EntityId] = Transform;
    Components->Drawables[EntityId] = drawable{ Texture };
    Components->Chests[EntityId] = chest{Value};
    Components->Targetables[EntityId] = targetable{false};
    Components->Colliders[EntityId] = MakeCollider(Transform);

    return NewEntity;
}

entity
AddResource(int EntityId, components *Components, transform Transform, std::string Texture, resource_type Type, int Amount)
{
    entity NewEntity = entity{ EntityId };

    Components->Transforms[EntityId] = Transform;
    Components->Drawables[EntityId] = drawable{ Texture };
    Components->Resources[EntityId] = resource{ Type, Amount };
    Components->Targetables[EntityId] = targetable{false};
    Components->Colliders[EntityId] = MakeCollider(Transform);

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
    Components->Targetables[EntityId] = targetable{false};
    Components->Colliders[EntityId] = MakeCollider(Transform);

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
    Components->Targetables[EntityId] = targetable{false};
    Components->Colliders[EntityId] = MakeCollider(Transform);

    return NewEntity;
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
            Buildings[Element.first].ProductionTimeLeft -= DeltaTime;
            if (Building.ProductionTimeLeft <= 0)
            {
                Building.InProduction = false;
                Building.ProductionTimeLeft = Building.ProductionTime;

                Buildings[Element.first] = Building;

                return true;
            }
        }
    }

    return false;
}

void
StartUnitProduction(std::unordered_map<int, building> &Buildings, std::unordered_map<int, targetable> Targetables)
{
    for (std::pair<int, building> Building : Buildings)
    {
        if (Targetables[Building.first].IsTargeted)
        {
            if (Building.second.InProduction)
                return;

            Building.second.InProduction = true;
            Buildings[Building.first] = Building.second;
            return;
        }
    }
}

void
StopUnitProduction(std::unordered_map<int, building> &Buildings)
{
    for (std::pair<int, building> Building : Buildings)
    {
        Buildings[Building.first].InProduction = false;
        Buildings[Building.first].ProductionTimeLeft = Buildings[Building.first].ProductionTime;
    }
}


void
UpdateCollider(transform Transform, collider &Collider)
{
    Collider = MakeCollider(Transform);
}

// NOTE(insolence): We check collisions between an entity that has just been moved and other colliders
bool
CurrentCollides(collider Current, int CurrentIndex, std::unordered_map<int, collider> Colliders)
{
    for (std::pair<int, collider> Collider : Colliders)
    {
        if (Collider.first == CurrentIndex)
            continue;

        if (Intersect(Current.Collider, Collider.second.Collider))
        {
            return true;
        }
    }

    return false;
}

// TODO(insolence): Now check what a unit collides with
void
MoveEntities(std::unordered_map<int, transform> &Transforms,
             std::unordered_map<int, motion>    &Motions,
             std::unordered_map<int, collider>  &Colliders, float DeltaTime)
{
    for (std::pair<int, motion> Motion : Motions)
    {
        int EntityIndex = Motion.first;

        float AbsXDiff = Abs(Motion.second.TargetPos.X - Transforms[Motion.first].Pos.X);
        float AbsYDiff = Abs(Motion.second.TargetPos.Y - Transforms[Motion.first].Pos.Y);

        if (AbsXDiff <= 0.02f && AbsYDiff <= 0.02f)
        {
            Transforms[EntityIndex].Pos = Motions[EntityIndex].TargetPos;
            Motions[EntityIndex].TargetPos = Transforms[EntityIndex].Pos;
            Motions[EntityIndex].Velocity = vec2{0, 0};
        }
        else
        {
            // NOTE(insolence): Now check collisions and, if any, don't move the object
            transform NewTransform = Transforms[EntityIndex];
            NewTransform.Pos = Transforms[EntityIndex].Pos + Motions[EntityIndex].Velocity * DeltaTime;
            collider NewCollider;
            UpdateCollider(NewTransform, NewCollider);

            if (CurrentCollides(NewCollider, EntityIndex, Colliders))
            {
                return;
            }

            Transforms[EntityIndex] = NewTransform;
            UpdateCollider(Transforms[EntityIndex], Colliders[EntityIndex]);
        }
    }
}

int
GetTargetedIndex(std::unordered_map<int, targetable> Targetables)
{
    for (std::pair<int, targetable> Targetable : Targetables)
    {
        if (Targetable.second.IsTargeted)
            return Targetable.first;
    }

    return -1;
}

void
SetUntargetableAll(std::unordered_map<int, targetable> &Targetables)
{
    for (std::pair<int, targetable> Targetable : Targetables)
    {
        // TODO(insolence): Probably revise the syntax, try to get the pair as a reference
        Targetables[Targetable.first].IsTargeted = false;
    }
}

void
TryToSetTargetable(std::unordered_map<int, targetable> &Targetables,
                   std::unordered_map<int, collider> Colliders, vec2 WorldCursorPos)
{
    for (std::pair<int, targetable> Element : Targetables)
    {
        if (IsPointInsideAABB(WorldCursorPos, Colliders[Element.first].Collider))
        {
            Targetables[Element.first] = targetable{true};
        }
        else
        {
            Targetables[Element.first] = targetable{false};
        }
    }
}

// TODO(insolence): Only move allied units
void
ChangeTargetedDest(std::unordered_map<int, targetable> Targetables, std::unordered_map<int, motion> &Motions, std::unordered_map<int, unit> Units,
                   std::unordered_map<int, transform> Transforms, std::unordered_map<int, collider> Colliders, vec2 WorldCursorPos)
{

    for (std::pair<int, unit> Unit : Units)
    {
        if (Targetables[Unit.first].IsTargeted)
        {
            // NOTE(insolence): Check when user clicks on the unit to prevent wrong move patterns
            if (IsPointInsideAABB(WorldCursorPos, Colliders[Unit.first].Collider))
                return;

            motion &MotionComp = Motions[Unit.first];
            MotionComp.TargetPos = vec2{WorldCursorPos.X-0.5f, WorldCursorPos.Y-0.5f};
            MotionComp.Velocity = Normalize(MotionComp.TargetPos - Transforms[Unit.first].Pos) * MotionComp.Speed;

            return;
        }
    }
}

void
DisplayDrawables(std::unordered_map<int, transform> Transforms,
                 std::unordered_map<int, drawable> Drawables,
                 std::unordered_map<int, targetable> Targetables,
                 std::vector<dir_light> DirLights, std::vector<point_light> PointLights, std::vector<spotlight_light> SpotLights,
                 orthographic_camera *Camera)
{
    for (std::pair<int, drawable> Element : Drawables)
    {
        transform TransformComp = Transforms[Element.first];
        if (Targetables[Element.first].IsTargeted)
        {
            DrawRectangleTextured(Camera, Transform(TransformComp.Pos, 0.f, TransformComp.Size/2.f), GetTexture(Element.second.Texture),
                                  GetNormalFromTexture(Element.second.Texture), DirLights, PointLights, SpotLights, color{0.5f, 0, 0});
        }
        else
        {
            DrawRectangleTextured(Camera, Transform(TransformComp.Pos, 0.f, TransformComp.Size/2.f), GetTexture(Element.second.Texture),
                                  GetNormalFromTexture(Element.second.Texture), DirLights, PointLights, SpotLights);
        }
    }
}

void
DisplayAttackerPowers(std::unordered_map<int, transform> Transforms, std::unordered_map<int, attacker> Attackers,
                      orthographic_camera *Camera, color Color = RED)
{
    for (std::pair<int, attacker> Attacker : Attackers)
    {
        std::string AttackerPowerStr = std::to_string(Attacker.second.Power);
        RenderText(Camera, AttackerPowerStr.c_str(), vec2{Transforms[Attacker.first].Pos.X + 0.05f, Transforms[Attacker.first].Pos.Y + 0.05f}, 1.5f, Color);
    }
}

void
DisplayProductionTime(std::unordered_map<int, transform> Transforms, std::unordered_map<int, building> Buildings,
                      orthographic_camera *Camera)
{
    for (std::pair<int, building> Building : Buildings)
    {
        std::string ProductionTimeLeftStr = std::to_string((int)Building.second.ProductionTimeLeft);
        RenderText(Camera, ProductionTimeLeftStr.c_str(), vec2{Transforms[Building.first].Pos.X - 0.23f, Transforms[Building.first].Pos.Y - 0.23f}, 1.5f, RED);
    }
}

