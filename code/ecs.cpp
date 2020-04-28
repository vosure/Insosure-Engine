#include "ecs.h"

collider MakeCollider(transform Transform)
{
    collider ColliderComponent;
    ColliderComponent.Collider.Min = Transform.Pos + vec2{0.1f, 0.1f};
    ColliderComponent.Collider.Max.X = Transform.Pos.X + Transform.Size - 0.1f;
    ColliderComponent.Collider.Max.Y = Transform.Pos.Y + Transform.Size - 0.1f;

    return ColliderComponent;
}

// NOTE(insolence): For now patrol radius is hardcoded
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
    Components->Patrols[EntityId] = patrol { Transform.Pos, 3, -1};

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

// NOTE(insolence): Probably revise
void
RemoveEntity(components *Components, int EntityIndex)
{
    Components->Transforms.erase(EntityIndex);
    Components->Motions.erase(EntityIndex);
    Components->Colliders.erase(EntityIndex);
    Components->Drawables.erase(EntityIndex);
    Components->Resources.erase(EntityIndex);
    Components->Buildings.erase(EntityIndex);
    Components->Targetables.erase(EntityIndex);
    Components->Healths.erase(EntityIndex);
    Components->Attackers.erase(EntityIndex);
    Components->Units.erase(EntityIndex);
    Components->Chests.erase(EntityIndex);
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
int
CurrentCollides(collider Current, int CurrentIndex, std::unordered_map<int, collider> Colliders)
{
    for (std::pair<int, collider> Collider : Colliders)
    {
        if (Collider.first == CurrentIndex)
            continue;

        if (Intersect(Current.Collider, Collider.second.Collider))
        {
            return Collider.first;
        }
    }

    return -1;
}

battlefield
CreateBattlefield(vec2 BattleLocation)
{
    battlefield Battlefield;
    Battlefield.BattleLocation = BattleLocation;
    Battlefield.PlayerPos = {10.f + XOFFSET, 7.f + YOFFSET};
    Battlefield.OldPlayerPos = {10.f + XOFFSET, 7.f + YOFFSET};

    float ZoomLevel = 8.f;
    Battlefield.BattleCamera.ZoomLevel = ZoomLevel;
    SetViewProjection(&Battlefield.BattleCamera, -ZoomLevel * RESOLUTION, ZoomLevel * RESOLUTION, -ZoomLevel, ZoomLevel);
    Battlefield.BattleCamera.Position = vec3{-12.7f - XOFFSET, -7.3f - YOFFSET, 0};

    return Battlefield;
}

void
UnitOnCollision(game_world &World, int UnitIndex, int CollisionObjIndex)
{
    if (World.Components.Chests.find(CollisionObjIndex) != World.Components.Chests.end())
    {
        World.Components.Attackers[UnitIndex].Power += World.Components.Chests[CollisionObjIndex].Value;
        RemoveEntity(&World.Components, CollisionObjIndex);
    }

    else if (World.Components.Attackers.find(CollisionObjIndex) != World.Components.Attackers.end() &&
        World.Components.Units.find(CollisionObjIndex) == World.Components.Units.end())
    {
        World.Mode = BATTLE;
        battlefield NewBattlefield = CreateBattlefield(World.Components.Transforms[UnitIndex].Pos);
        World.ActiveBattlefields.push_back(NewBattlefield);
        World.ActiveBattleNum = World.ActiveBattlefields.size() - 1;
        MusicSwitched = false;
        RemoveEntity(&World.Components, CollisionObjIndex);
    }
}

void
MoveEntities(game_world &World, float DeltaTime)
{
    components *Components = &World.Components;

    for (std::pair<int, motion> Motion : Components->Motions)
    {
        int EntityIndex = Motion.first;

        float AbsXDiff = Abs(Motion.second.TargetPos.X - Components->Transforms[Motion.first].Pos.X);
        float AbsYDiff = Abs(Motion.second.TargetPos.Y - Components->Transforms[Motion.first].Pos.Y);

        if (AbsXDiff <= 0.02f && AbsYDiff <= 0.02f)
        {
            Components->Transforms[EntityIndex].Pos = Components->Motions[EntityIndex].TargetPos;
            Components->Motions[EntityIndex].TargetPos = Components->Transforms[EntityIndex].Pos;
            Components->Motions[EntityIndex].Velocity = vec2{0, 0};
        }
        else
        {
            // NOTE(insolence): Now check collisions and, if any, don't move the object
            transform NewTransform = Components->Transforms[EntityIndex];
            NewTransform.Pos = Components->Transforms[EntityIndex].Pos + Components->Motions[EntityIndex].Velocity * DeltaTime;
            collider NewCollider;
            UpdateCollider(NewTransform, NewCollider);

            int CurrentCollisionEntity = CurrentCollides(NewCollider, EntityIndex, Components->Colliders);
            if (CurrentCollisionEntity != -1)
            {
                if (Components->Units.find(EntityIndex) != Components->Units.end())
                {
                    UnitOnCollision(World, EntityIndex, CurrentCollisionEntity);
                }
                continue;
            }

            Components->Transforms[EntityIndex] = NewTransform;
            UpdateCollider(Components->Transforms[EntityIndex], Components->Colliders[EntityIndex]);
        }
    }
}

void
UpdatePatrolRoutes(std::unordered_map<int, patrol> Patrols, std::unordered_map<int, motion> &Motions,
                   std::unordered_map<int, transform> Transforms, std::unordered_map<int, unit> Units)
{
    for (std::pair<int, patrol> Patrol : Patrols)
    {
        int Index = Patrol.first;

        for (std::pair<int, unit> Unit : Units)
        {
            int UnitIndex = Unit.first;
            if (Patrols[Index].ChasingEntity == -1 &&
                IsPointInsideCircle(Transforms[UnitIndex].Pos, bounding_circle{Transforms[Index].Pos, Patrols[Index].Radius}))
            {
                Motions[Index].TargetPos = Transforms[UnitIndex].Pos;
                Motions[Index].Velocity = Normalize(Motions[Index].TargetPos - Transforms[Index].Pos) * Motions[Index].Speed;
                Patrols[Index].ChasingEntity = UnitIndex;
            }
        }

        if (Patrols[Index].ChasingEntity == -1 && Transforms[Index].Pos == Motions[Index].TargetPos || Motions[Index].TargetPos == vec2{-1, -1})
        {
            Motions[Index].TargetPos = GetRandomPointInCircle(Patrol.second.InitialPos, Patrol.second.Radius);
            Motions[Index].Velocity = Normalize(Motions[Index].TargetPos - Transforms[Index].Pos) * Motions[Index].Speed;
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

