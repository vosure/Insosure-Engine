#include "main.h"

#include <glad/src/glad.c>
#include <glfw/include/glfw3.h>
#include <stb_image/stb_image.cpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <irrKlang/include/irrKlang.h>

//#include "utils/string.cpp"
#include "utils/file_utils.h"
#include "math/math.h"
#include "random/random.h"
#include "math/perlin.h"
#include "utils/hash_map.cpp"
#include "utils/array_list.h"
#include "utils/time.h"
#include "utils/colors.h"

#include "debug.h"
#include "render/texture.h"
#include "render/sprite.h"
#include "physics/physics.h"
#include "render/framebuffer.h"
#include "render/light.h"
#include "render/text.h"
#include "render/particle_system.h"
#include "render/render.cpp"
#include "render/gui_render.cpp"
#include "input/input.h"
#include "window.h"

global_variable bool MusicSwitched = false;

global_variable orthographic_camera GlobalCamera;

vec2 GetCursorWorldPos(GLFWwindow *Window, orthographic_camera *Camera)
{
    vec2 CursorPos = GetCursorPos(Window);

    vec2 CursorWorldPos = vec2{ CursorPos.X/CurrentWidth, 1 - CursorPos.Y/CurrentHeight };
    CursorWorldPos = vec2{
        MapFloat(0, 1, -Camera->ZoomLevel*RESOLUTION, Camera->ZoomLevel*RESOLUTION, CursorWorldPos.X) - Camera->Position.X + 0.5f,
        MapFloat(0, 1, -Camera->ZoomLevel, Camera->ZoomLevel, CursorWorldPos.Y) - Camera->Position.Y + 0.5f
    };

    return CursorWorldPos;
}

global_variable bool PlayerMoved = false;
void
ProcessInput(GLFWwindow *Window, orthographic_camera *Camera, game_world *World, float Dt)
{
    // NOTE(insolence): Fullscreen
    // TODO(vosure): Change glfw key names to custom
    if (IsKeyPressed(GLFW_KEY_F12) && !IsKeyProcessed(GLFW_KEY_F12))
    {
        SwitchFullscreen(Window);
        KeyboardInput.KeysProcessed[GLFW_KEY_F12] = true;
    }


    if (World->Mode == GLOBAL)
    {
        float CameraSpeed = 4.f;
        if (IsKeyPressed(GLFW_KEY_UP))
        {
            Camera->Position.Y -= CameraSpeed * Dt;
        }
        else if (IsKeyPressed(GLFW_KEY_DOWN))
        {
            Camera->Position.Y += CameraSpeed * Dt;
        }
        if (IsKeyPressed(GLFW_KEY_LEFT))
        {
            Camera->Position.X += CameraSpeed * Dt;
        }
        else if (IsKeyPressed(GLFW_KEY_RIGHT))
        {
            Camera->Position.X -= CameraSpeed * Dt;
        }

        // NOTE(insolence): Moving the camera with the mouse cursor
        float ScrollingSpeed = 1.5f;
        vec2 MousePosition = GetCursorPos(Window);
        if (MousePosition.X <= 75)
        {
            Camera->Position.X += CameraSpeed * Dt * ScrollingSpeed;
        }
        if (MousePosition.Y <= 75)
        {
            Camera->Position.Y -= CameraSpeed * Dt * ScrollingSpeed;
        }
        if (MousePosition.X >= CurrentWidth - 75)
        {
            Camera->Position.X -= CameraSpeed * Dt * ScrollingSpeed;
        }
        if (MousePosition.Y >= CurrentHeight - 75)
        {
            Camera->Position.Y += CameraSpeed * Dt * ScrollingSpeed;
        }

        // TODO(insolence): Consider enabling camera rotation
        // if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
        // {
        //     Camera->Rotation += 1.f;
        // }
        // else if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
        // {
        //     Camera->Rotation -= 1.f;
        // }

        // NOTE(insolence): Mouse input
        if (IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && !IsMouseButtonProcessed(GLFW_MOUSE_BUTTON_LEFT))
        {
            vec2 CursorWorldPos = GetCursorWorldPos(Window, Camera);
            for (int i = 0; i < World->Player.UnitsNum; i++)
            {
                if (IsPointInsideAABB(CursorWorldPos, World->Player.Units[i].Collider))
                {
                    World->Player.UnitChosen = i;
                    break;
                }
                else
                {
                    World->Player.UnitChosen = NO_UNIT;
                }
            }
            for (int i = 0; i < World->Objects.Buildings.size(); i++)
            {
                World->Objects.Buildings[i].Chosen = false;

                if (IsPointInsideAABB(CursorWorldPos, World->Objects.Buildings[i].Collider))
                {
                    World->Objects.Buildings[i].Chosen = true;
                }
            }

        }
        if (IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT) && !IsMouseButtonProcessed(GLFW_MOUSE_BUTTON_RIGHT))
        {
            int CurrentUnit = World->Player.UnitChosen;
            if (CurrentUnit != NO_UNIT)
            {
                vec2 CursorWorldPos = GetCursorWorldPos(Window, Camera);
                if (CursorWorldPos.X >= 0 && CursorWorldPos.X < WORLD_WIDTH && CursorWorldPos.Y >= 0 && CursorWorldPos.Y < WORLD_HEIGHT &&
                    !IsPointInsideAABB(CursorWorldPos, World->Player.Units[CurrentUnit].Collider))
                {
                    World->Player.Units[CurrentUnit].TargetPos = vec2{CursorWorldPos.X-0.5f, CursorWorldPos.Y-0.5f};
                    // FIXME(insolence): Check when user clicks on the unit
                    World->Player.Units[CurrentUnit].Velocity = Normalize(World->Player.Units[CurrentUnit].TargetPos - World->Player.Units[CurrentUnit].Pos) *
                                                                World->Player.Units[CurrentUnit].Speed;
                }

                MouseInput.ButtonsProcessed[GLFW_MOUSE_BUTTON_RIGHT] = true;
            }
        }
        if (IsKeyPressed(GLFW_KEY_H) && !IsKeyProcessed(GLFW_KEY_H))
        {
            if (World->Player.UnitChosen != NO_UNIT)
            {
                World->Player.Units[World->Player.UnitChosen].Velocity = vec2{0, 0};
                World->Player.Units[World->Player.UnitChosen].TargetPos = World->Player.Units[World->Player.UnitChosen].Pos;
            }
        }

        if (IsKeyPressed(GLFW_KEY_B) && !IsKeyProcessed(GLFW_KEY_B))
        {
            for (int i = 0; i < World->Objects.Buildings.size(); i++)
            {
                if (World->Objects.Buildings[i].Chosen && !World->Objects.Buildings[i].InProduction)
                {
                    World->Objects.Buildings[i].InProduction = true;
                    World->Objects.Buildings[i].ProductionTimeLeft = 10.f;
                }
            }
        }
    }

    if (World->Mode == BATTLE)
    {
        if (IsKeyPressed(GLFW_KEY_ESCAPE) && !IsKeyProcessed(GLFW_KEY_ESCAPE))
        {
            World->ActiveBattleNum = NO_BATTLE;
            World->Mode = GLOBAL;
            MusicSwitched = false;
        }

        battlefield ActiveBattle = World->ActiveBattlefields[World->ActiveBattleNum];

        if (IsKeyPressed(GLFW_KEY_W) && !IsKeyProcessed(GLFW_KEY_W))
        {
            World->ActiveBattlefields[World->ActiveBattleNum].OldPlayerPos = World->ActiveBattlefields[World->ActiveBattleNum].PlayerPos;
            if (World->ActiveBattlefields[World->ActiveBattleNum].PlayerPos.Y < FIELD_HEIGHT-1 + YOFFSET)
            {
                World->ActiveBattlefields[World->ActiveBattleNum].PlayerPos.Y++;
                PlayerMoved = true;
            }
            KeyboardInput.KeysProcessed[GLFW_KEY_W] = true;
        }
        else if (IsKeyPressed(GLFW_KEY_S) && !IsKeyProcessed(GLFW_KEY_S))
        {
            World->ActiveBattlefields[World->ActiveBattleNum].OldPlayerPos = World->ActiveBattlefields[World->ActiveBattleNum].PlayerPos;
            if (World->ActiveBattlefields[World->ActiveBattleNum].PlayerPos.Y > 0 + YOFFSET)
            {
                World->ActiveBattlefields[World->ActiveBattleNum].PlayerPos.Y--;
                PlayerMoved = true;
            }
            KeyboardInput.KeysProcessed[GLFW_KEY_S] = true;
        }
        if (IsKeyPressed(GLFW_KEY_A) && !IsKeyProcessed(GLFW_KEY_A))
        {
            World->ActiveBattlefields[World->ActiveBattleNum].OldPlayerPos = World->ActiveBattlefields[World->ActiveBattleNum].PlayerPos;
            if (World->ActiveBattlefields[World->ActiveBattleNum].PlayerPos.X > 0 + XOFFSET)
            {
                World->ActiveBattlefields[World->ActiveBattleNum].PlayerPos.X--;
                PlayerMoved = true;
            }
            KeyboardInput.KeysProcessed[GLFW_KEY_A] = true;
        }
        else if (IsKeyPressed(GLFW_KEY_D) && !IsKeyProcessed(GLFW_KEY_D))
        {
            World->ActiveBattlefields[World->ActiveBattleNum].OldPlayerPos = World->ActiveBattlefields[World->ActiveBattleNum].PlayerPos;
            if (World->ActiveBattlefields[World->ActiveBattleNum].PlayerPos.X < FIELD_WIDTH-1 + XOFFSET)
            {
                World->ActiveBattlefields[World->ActiveBattleNum].PlayerPos.X++;
                PlayerMoved = true;
            }
            KeyboardInput.KeysProcessed[GLFW_KEY_D] = true;
        }
    }

    if (World->Mode == MENU)
    {
        // TODO(insolence): Do smth here
    }
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

// HACK(insolence): Temp solution
#define NULL_POS vec2{-40, -40}
void
NullifyEnemy(enemy *Enemy)
{
    Enemy->Pos = NULL_POS;
}

void
CheckCollisions(game_world *World)
{
    for (int i = 0; i < World->Player.UnitsNum; i++)
    {
        for (int UnitIndex = i + 1; UnitIndex < World->Player.UnitsNum; UnitIndex++)
        {
            if (Intersect(World->Player.Units[i].Collider, World->Player.Units[UnitIndex].Collider))
            {
                World->Player.Units[i].TargetPos = World->Player.Units[i].Pos;
                World->Player.Units[UnitIndex].TargetPos = World->Player.Units[UnitIndex].Pos;
            }
        }

        for (int EnemyIndex = 0; EnemyIndex < World->Objects.Enemies.size(); EnemyIndex++)
        {
            if (World->Objects.Enemies[EnemyIndex].Pos != NULL_POS && Intersect(World->Player.Units[i].Collider, World->Objects.Enemies[EnemyIndex].Collider))
            {
                battlefield NewBattlefield = CreateBattlefield(World->Player.Units[i].Pos);
                World->ActiveBattlefields.push_back(NewBattlefield);

                World->ActiveBattleNum = World->ActiveBattlefields.size() - 1;

                World->Mode = BATTLE;
                MusicSwitched = false;

                NullifyEnemy(&World->Objects.Enemies[EnemyIndex]);
            }
        }

        for (int ObstacleIndex = 0; ObstacleIndex < World->Objects.Obstacles.size(); ObstacleIndex++)
        {
            if (Intersect(World->Player.Units[i].Collider, World->Objects.Obstacles[ObstacleIndex].Collider))
            {
                World->Player.Units[i].TargetPos = World->Player.Units[i].Pos;
            }
        }

        for (int ChestIndex = 0; ChestIndex < World->Objects.Chests.size(); ChestIndex++)
        {
            if (World->Objects.Chests[ChestIndex].Pos != NULL_POS && Intersect(World->Player.Units[i].Collider, World->Objects.Chests[ChestIndex].Collider))
            {
                //World->Player.Units[i].TargetPos = World->Player.Units[i].Pos;
                World->Player.Units[i].Power += World->Objects.Chests[ChestIndex].Value;
                World->Objects.Chests[ChestIndex].Pos = NULL_POS;
            }
        }
        for (int BuildingIndex = 0; BuildingIndex < World->Objects.Buildings.size(); BuildingIndex++)
        {
            if (Intersect(World->Player.Units[i].Collider, World->Objects.Buildings[BuildingIndex].Collider))
            {
                World->Player.Units[i].TargetPos = World->Player.Units[i].Pos;
            }
        }
    }

}

void
OnCollision()
{
    // TODO(insolence): Move some code from CheckCollisions there
}

void
UpdatePositions(game_world *World, float DeltaTime)
{
    for (int i = 0; i < World->Player.UnitsNum; i++)
    {
        float AbsXDiff = Abs(World->Player.Units[i].TargetPos.X - World->Player.Units[i].Pos.X);
        float AbsYDiff = Abs(World->Player.Units[i].TargetPos.Y - World->Player.Units[i].Pos.Y);

        if (AbsXDiff <= 0.05f && AbsYDiff <= 0.05f)
        {
           World->Player.Units[i].Velocity = vec2{0.f, 0.f};
           World->Player.Units[i].Pos = World->Player.Units[i].TargetPos;
        }
        else
            World->Player.Units[i].Pos += World->Player.Units[i].Velocity * DeltaTime;

        UpdateAABB(&World->Player.Units[i].Collider, World->Player.Units[i].Pos, World->Player.Units[i].Size);
    }

    // TODO(insolence): Update entities
    for (int i = 0; i < World->Objects.Enemies.size(); i++)
    {
        float AbsXDiff = Abs(World->Objects.Enemies[i].TargetPos.X - World->Objects.Enemies[i].Pos.X);
        float AbsYDiff = Abs(World->Objects.Enemies[i].TargetPos.Y - World->Objects.Enemies[i].Pos.Y);

        if (AbsXDiff <= 0.05f && AbsYDiff <= 0.05f)
        {
           World->Objects.Enemies[i].Velocity = vec2{0.f, 0.f};
           World->Objects.Enemies[i].Pos = World->Objects.Enemies[i].TargetPos;
        }
        else
            World->Objects.Enemies[i].Pos += World->Objects.Enemies[i].Velocity * World->Objects.Enemies[i].Speed * DeltaTime;

        UpdateAABB(&World->Objects.Enemies[i].Collider, World->Objects.Enemies[i].Pos, World->Objects.Enemies[i].Size);
    }

    // NOTE(insolence): Updating buildings, so rename a function
    for (int i = 0; i < World->Objects.Buildings.size(); i++)
    {
        if (World->Objects.Buildings[i].InProduction)
        {
            World->Objects.Buildings[i].ProductionTimeLeft -= DeltaTime;
            if (World->Objects.Buildings[i].ProductionTimeLeft <= 0.f)
            {
                World->Objects.Buildings[i].InProduction = false;
                World->Player.UnitsNum++;

                unit Unit;

                Unit.Pos = vec2{World->Objects.Buildings[i].Pos.X - 1.f, World->Objects.Buildings[i].Pos.Y - 1.f};
                Unit.Power = 32;
                Unit.Size = 1.f;
                Unit.Speed = 2.f;
                UpdateAABB(&Unit.Collider, Unit.Pos, Unit.Size);
                Unit.Texture = String("archer.png");
                Unit.NormalTexture = String("archer_normal.png");

                World->Player.Units.push_back(Unit);
            }
        }
    }


}

// NOTE(insolence): Range is -1,1 -1,1
void
RenderGlobalMapGui(game_world *World)
{
    DrawTexturedRectangleOnScreen(vec2{-1.f, -1.f}, vec2{1.f, -0.4f}, GetTexture("mist.jpg"));

    if (World->Player.UnitChosen != NO_UNIT)
    {
        DrawRectangleOnScreen(vec2{-1.f, -1.f}, vec2{-0.6f, -0.4f}, SILVER);
        DrawTexturedRectangleOnScreen(vec2{-1.f, -1.f}, vec2{-0.6f, -0.4f}, GetTexture("frame.png"));
        DrawTexturedRectangleOnScreen(vec2{-0.9f, -0.9f}, vec2{-0.7f, -0.5f}, GetTexture(World->Player.Units[World->Player.UnitChosen].Texture.Native));
    }

    for (int BuildingIndex = 0; BuildingIndex < World->Objects.Buildings.size(); BuildingIndex++)
        {
            if (World->Objects.Buildings[BuildingIndex].Chosen)
            {
                DrawRectangleOnScreen(vec2{-1.f, -1.f}, vec2{-0.6f, -0.4f}, SILVER);
                DrawTexturedRectangleOnScreen(vec2{-1.f, -1.f}, vec2{-0.6f, -0.4f}, GetTexture("frame.png"));
                DrawTexturedRectangleOnScreen(vec2{-0.9f, -0.9f}, vec2{-0.7f, -0.5f}, GetTexture(World->Objects.Buildings[BuildingIndex].Texture.Native));
            }
        }
}

void
RenderGlobalMap(game_world *World, orthographic_camera *Camera, std::vector<dir_light> DirLights, std::vector<point_light> PointLights, std::vector<spotlight_light> SpotLights)
{
    // FIXME(insolence): Change to dynamic
    string EnemyPowerStr = IntToStr(26);
    string ChestValueStr = IntToStr(5);

    vec2 CurrentTilePos = { -Camera->Position.X, -Camera->Position.Y};

    // NOTE(insolence): Rendering the ground around the player depending on camera pos
    for (int Y = CurrentTilePos.Y - (Camera->ZoomLevel+3); Y < CurrentTilePos.Y + (Camera->ZoomLevel+3); Y++)
    {
        for (int X = CurrentTilePos.X - Camera->ZoomLevel*RESOLUTION*2; X < CurrentTilePos.X + Camera->ZoomLevel*RESOLUTION*2; X++)
        {
            if (X < 0 || Y < 0 || X > WORLD_WIDTH || Y > WORLD_HEIGHT)
                continue;

            DrawRectangleTextured(Camera, Transform(vec2{(float)X, (float)Y}, 0.f, 0.5f),
                                  GetTexture("rock.png"), GetNormal("rock_normal.png"), DirLights, PointLights, SpotLights);
        }
    }

    for (int i = 0; i < World->Objects.Enemies.size(); i++)
    {
        enemy Enemy = World->Objects.Enemies[i];
        DrawRectangleTextured(Camera, Transform(vec2{Enemy.Pos.X, Enemy.Pos.Y}, 0.f, Enemy.Size/2.f),
                              GetTexture("monster.png"), GetNormal("monster_normal.png"), DirLights, PointLights, SpotLights);
        RenderText(Camera, EnemyPowerStr.Native, World->Objects.Enemies[i].Pos.X + 0.05f, World->Objects.Enemies[i].Pos.Y + 0.05f, 1.5f, RED);

    }
    for (int i = 0; i < World->Objects.Obstacles.size(); i++)
    {
        obstacle Obstacle = World->Objects.Obstacles[i];
        DrawRectangleTextured(Camera, Transform(vec2{Obstacle.Pos.X, Obstacle.Pos.Y}, 0.f, Obstacle.Size/2.f),
                              GetTexture("rocks.png"), GetNormal("rocks_normal.png"), DirLights, PointLights, SpotLights);
    }
    for (int i = 0; i < World->Objects.Chests.size(); i++)
    {
        chest Chest = World->Objects.Chests[i];
        DrawRectangleTextured(Camera, Transform(vec2{Chest.Pos.X, Chest.Pos.Y}, 0.f, Chest.Size/2.f),
                              GetTexture("treasure.png"), GetNormal("treasure_normal.png"), DirLights, PointLights, SpotLights);
        RenderText(Camera, ChestValueStr.Native, World->Objects.Chests[i].Pos.X + 0.05f, World->Objects.Chests[i].Pos.Y + 0.05f, 1.5f, RED);
    }
    for (int i = 0; i < World->Objects.Buildings.size(); i++)
    {
        building Building = World->Objects.Buildings[i];
        DrawRectangleTextured(Camera, Transform(vec2{Building.Pos.X, Building.Pos.Y}, 0.f, Building.Size/2.f),
                              GetTexture(Building.Texture.Native), GetNormal(Building.NormalTexture.Native), DirLights, PointLights, SpotLights);
    }

    for (int i = 0; i < World->Player.UnitsNum; i++)
    {
        if (World->Player.UnitChosen == i)
        {
            DrawRectangleTextured(Camera, Transform(vec2{World->Player.Units[i].Pos.X, World->Player.Units[i].Pos.Y}, 0.f, World->Player.Units[i].Size/2.f),
                              GetTexture(World->Player.Units[i].Texture.Native), GetNormal(World->Player.Units[i].NormalTexture.Native), DirLights, PointLights, SpotLights, color{0.15f, 0, 0});
        }
        else
        {
            DrawRectangleTextured(Camera, Transform(vec2{World->Player.Units[i].Pos.X, World->Player.Units[i].Pos.Y}, 0.f, World->Player.Units[i].Size/2.f),
                              GetTexture(World->Player.Units[i].Texture.Native), GetNormal(World->Player.Units[i].NormalTexture.Native), DirLights, PointLights, SpotLights);
        }
        string PlayerPowerStr = IntToStr(World->Player.Units[i].Power);
        RenderText(Camera, PlayerPowerStr.Native, World->Player.Units[i].Pos.X + 0.05f, World->Player.Units[i].Pos.Y + 0.05f, 1.5f, RED);
        FreeString(PlayerPowerStr);
    }

    RenderGlobalMapGui(World);

    FreeString(EnemyPowerStr);
    FreeString(ChestValueStr);
}

void
RenderBattlefieldGui(battlefield *CurrentBattlefield)
{
    DrawRectangleOnScreen(vec2{-0.6f, -0.9f}, vec2{0.6f, -0.83f}, RED); // NOTE(insolence): Health bar for now
}

void RenderBattlefield(game_world *World,std::vector<particle> Particles)
{
    if (World->ActiveBattleNum == -1)
        return;

    battlefield ActiveBattle = World->ActiveBattlefields[World->ActiveBattleNum];

    float ZoomLevel = ActiveBattle.BattleCamera.ZoomLevel;
    for (int Y = 0 + YOFFSET; Y < 15 + YOFFSET; Y++)
    {
        for (int X = 0 + XOFFSET; X < 25 + XOFFSET; X++)
        {
            DrawRectangleTextured(&ActiveBattle.BattleCamera, Transform(vec2{(float)X, (float)Y}, 0.f, 0.5f),
                                  GetTexture("rock.png"), GetNormal("rock_normal.png"), std::vector<dir_light>(), std::vector<point_light>(), std::vector<spotlight_light>());

            if ((int)ActiveBattle.PlayerPos.X == X && (int)ActiveBattle.PlayerPos.Y == Y)
            {
                DrawRectangleTextured(&ActiveBattle.BattleCamera, Transform(vec2{(float)X, (float)Y}, 0.f, 0.5f),
                                      GetTexture(World->Player.Units[World->Player.UnitChosen].Texture.Native), GetNormal(World->Player.Units[World->Player.UnitChosen].NormalTexture.Native),
                                      std::vector<dir_light>(), std::vector<point_light>(), std::vector<spotlight_light>());
            }
        }
    }

    RenderBattlefieldGui(&ActiveBattle);

    DrawParticles(&ActiveBattle.BattleCamera, Particles);
}

void
UpdateAndRender(GLFWwindow *Window, orthographic_camera *Camera, postprocessing_effects *Effects)
{
    float DeltaTime = 0.f;
    float LastFrame = 0.f;

    irrklang::ISound *MainMusic;
    irrklang::ISound *BattleMusic;

    irrklang::ISoundEngine *SoundEngine = irrklang::createIrrKlangDevice();
    SoundEngine->setSoundVolume(0.08f);

    game_world World = {};
    World.Mode = GLOBAL;

    for (int i = 0; i < World.Player.UnitsNum; i++)
    {
        unit Unit;
        Unit.Pos = vec2{(float)i, 0.f};
        Unit.Power = 32;
        Unit.Size = 1.f;
        Unit.Speed = 2.f;
        UpdateAABB(&Unit.Collider, Unit.Pos, Unit.Size);

        World.Player.Units.push_back(Unit);
    }
    World.Player.Units[0].Texture = String("roflanface.png");
    World.Player.Units[0].NormalTexture = String("roflanface_normal.png");
    World.Player.Units[1].Texture = String("archer.png");
    World.Player.Units[1].NormalTexture = String("archer_normal.png");
    World.Player.UnitChosen = NO_UNIT;

    for (int i = 0; i < 10; i++)
    {
        enemy Enemy;
        Enemy.Pos = vec2{GetRandomFloat(0, 50), GetRandomFloat(0, 50)};
        Enemy.TargetPos = Enemy.Pos;
        Enemy.Velocity = vec2{0.f, 0.f};
        Enemy.Power = 25;
        Enemy.Size = 1.f;
        Enemy.Speed = 2.f;
        UpdateAABB(&Enemy.Collider, Enemy.Pos, Enemy.Size);
        World.Objects.Enemies.push_back(Enemy);
    }
    for (int i = 0; i < 10; i++)
    {
        chest Chest;
        Chest.Pos = vec2{GetRandomFloat(0, 50), GetRandomFloat(0, 50)};
        Chest.Size = 1.f;
        Chest.Value = 5;
        UpdateAABB(&Chest.Collider, Chest.Pos, Chest.Size);
        World.Objects.Chests.push_back(Chest);
    }
    for (int i = 0; i < 10; i++)
    {
        obstacle Obstacle;
        Obstacle.Pos = vec2{GetRandomFloat(0, 50), GetRandomFloat(0, 50)};
        Obstacle.Size = 1.f;
        UpdateAABB(&Obstacle.Collider, Obstacle.Pos, Obstacle.Size);
        World.Objects.Obstacles.push_back(Obstacle);
    }

    building Building;
    Building.Pos = vec2{7, 7};
    Building.Size = 2.f;
    Building.InProduction = false;
    Building.ProductionTimeLeft = 15.f;
    Building.Type = "Barracks";
    Building.Texture = String("barracks.png");
    Building.NormalTexture = String("barracks_normal.png");
    UpdateAABB(&Building.Collider, Building.Pos, Building.Size);
    World.Objects.Buildings.push_back(Building);

    std::vector<dir_light> DirLights;
    std::vector<point_light> PointLights;
    std::vector<spotlight_light> SpotLights;

    color Ambient = CORAL * 0.05f;
    color Specular = WHITE_SMOKE;

    vec3 CLQ = {1.0f, 0.09f, 0.032f};

    PointLights.push_back(PointLight({5.0f, 5.0f, 1.0f}, Ambient, MAGENTA*3, Specular, CLQ));
    PointLights.push_back(PointLight({4.f, 4.f, 1.f}, Ambient, ORANGE*3, Specular, CLQ));
    PointLights.push_back(PointLight({6.f, 6.f, 1.f}, Ambient, AQUA*3, Specular, CLQ));
    PointLights.push_back(PointLight({2.f, 8.f, 1.f}, Ambient, LIME*3, Specular, CLQ));
    PointLights.push_back(PointLight({15.f, 8.f, 1.f}, Ambient, VIOLET*3, Specular, CLQ));
    PointLights.push_back(PointLight({3.f, 15.f, 1.f}, Ambient, GOLD*3, Specular, CLQ));


    std::vector<particle> Particles(100);

    // NOTE(insolence): Main rendering loop
    while (!glfwWindowShouldClose(Window))
    {

        float CurrentFrame = glfwGetTime();
        DeltaTime = CurrentFrame - LastFrame;
        LastFrame = CurrentFrame;

        ProcessInput(Window, Camera, &World, DeltaTime);

        glBindFramebuffer(GL_FRAMEBUFFER, HdrFB.ID);

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.01f, 0.01f, 0.01f, 1.f);

        UpdatePositions(&World, DeltaTime);

        if (World.Mode == GLOBAL)
        {
            if (!MusicSwitched)
            {
                SoundEngine->stopAllSounds();
                SoundEngine->play2D("W:/Insosure-Engine/assets/audio/onward.mp3", true);
                MusicSwitched = true;
            }

            RecalculateViewMatrix(Camera);
            CheckCollisions(&World);
            RenderGlobalMap(&World, Camera, DirLights, PointLights, SpotLights);
        }
        else if (World.Mode == BATTLE)
        {
            if (!MusicSwitched)
            {
                SoundEngine->stopAllSounds();
                SoundEngine->play2D("W:/Insosure-Engine/assets/audio/battle.mp3", true);
                MusicSwitched = true;
            }

            if (PlayerMoved)
            {
                for (int i = 0; i < 30; i++)
                {
                    Particles.push_back(SpawnParticle(World.ActiveBattlefields[World.ActiveBattleNum].OldPlayerPos, 0.5f, {GetRandomFloat(-0.2f, 0.2f), GetRandomFloat(-0.2f, 0.2f)}, GetRandomFloat(-45, 45), 2.5f, GetRandomFloat(0, 1) * 0.5f, color{4.3f, 2.2f, 0.f}, color{0.7f, 1.3f, 3.2f}));
                    PlayerMoved = false;
                }
            }

            UpdateParticles(Particles, DeltaTime);
            RecalculateViewMatrix(&World.ActiveBattlefields[World.ActiveBattleNum].BattleCamera);
            RenderBattlefield(&World, Particles);
        }

        string FpsStr = "FPS: " + FloatToStr(1.f/DeltaTime, 2);
        string MSPerFrameStr = "MS per frame: " + FloatToStr(DeltaTime * 1000, 2);
        RenderTextOnScreen(FpsStr.Native, 20.f, 2500.f, 1.f, {4, 1, 1});
        RenderTextOnScreen(MSPerFrameStr.Native, 20.f, 2420.f, 1.f, {4, 1, 1});
        FreeString(FpsStr);
        FreeString(MSPerFrameStr);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        Blur(&HdrFB, &PingpongFB[0]);

        glBindFramebuffer(GL_FRAMEBUFFER, PostprocessingFB.ID);
        ApplyHDR(HdrFB.TextureAttachment[0], PingpongFB[0].TextureAttachment[0], 1.0f);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        PostprocessScreenTexture(PostprocessingFB.TextureAttachment[0], *Effects);

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }
}

void ScrollCallback(GLFWwindow* Window, double XOffset, double YOffset)
{
    GlobalCamera.ZoomLevel -= YOffset/2;
    if (GlobalCamera.ZoomLevel <= 0.3f)
        GlobalCamera.ZoomLevel = 0.3f;
    else if (GlobalCamera.ZoomLevel >= 8)
        GlobalCamera.ZoomLevel = 8;

    float AspectRatio = 16.f / 9.f;
    SetViewProjection(&GlobalCamera, -GlobalCamera.ZoomLevel * AspectRatio, GlobalCamera.ZoomLevel * AspectRatio, -GlobalCamera.ZoomLevel, GlobalCamera.ZoomLevel);
}

void main()
{
    if (!glfwInit())
    {
        printf("Can't load glfw!");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4.6);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4.6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#if DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    GLFWwindow *Window = SetUpWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Heroes Budget Version", false);
    if (!Window)
    {
        printf("Failed to create the GLFW Window!");
        return;
    }
    GLFWmonitor *Primary = glfwGetPrimaryMonitor();
    const GLFWvidmode *Mode = glfwGetVideoMode(Primary);
    SCREEN_WIDTH = Mode->width;
    SCREEN_HEIGHT = Mode->height;

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD!");
        return;
    }
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

#if DEBUG
    GLint Flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &Flags);
    if (Flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        printf("Debug context initialized!\n");
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
    }
#endif

    LoadFreetype();
    TextureCache = CreateHashMap<const char*, uint>();
    MakeFramebuffers(WINDOW_WIDTH, WINDOW_HEIGHT);
    MakeShaders();

    // NOTE(insolence): We cull back faces specified in CCW order,
    // Front faces are in CW order
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float ZoomLevel = 4.f;
    GlobalCamera.ZoomLevel = ZoomLevel;
    SetViewProjection(&GlobalCamera, -ZoomLevel * RESOLUTION, ZoomLevel * RESOLUTION, -ZoomLevel, ZoomLevel); // NOTE(insolence): The ratio must be 16/9 in order to preserve the shapes
    GlobalCamera.Position = vec3{-4.5f, -3.f, 0}; //NOTE(vosure): check starting camera position

    glfwSetScrollCallback(Window, ScrollCallback);

    postprocessing_effects Effects;
    Effects.Inversion = false;
    Effects.Grayscale = false;
    Effects.Blur = false;

    UpdateAndRender(Window, &GlobalCamera, &Effects);

    glfwTerminate();
}