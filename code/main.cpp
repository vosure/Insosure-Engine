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

#include "debug.h"
#include "render/texture.h"
#include "render/sprite.h"
#include "physics/physics.h"
#include "render/framebuffer.h"
#include "render/text.h"
#include "render/particle_system.h"
#include "render/render.cpp"
#include "render/gui_render.cpp"
#include "input/input.h"
#include "window.h"


#include "ecs.cpp"

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

    if (IsKeyPressed(GLFW_KEY_F9) && !IsKeyProcessed(GLFW_KEY_F9))
    {
        Effects.Inversion = !Effects.Inversion;
        KeyboardInput.KeysProcessed[GLFW_KEY_F9] = true;
    }
    if (IsKeyPressed(GLFW_KEY_F10) && !IsKeyProcessed(GLFW_KEY_F10))
    {
        Effects.Grayscale = !Effects.Grayscale;
        KeyboardInput.KeysProcessed[GLFW_KEY_F10] = true;
    }
    if (IsKeyPressed(GLFW_KEY_F11) && !IsKeyProcessed(GLFW_KEY_F11))
    {
        Effects.Blur = !Effects.Blur;
        KeyboardInput.KeysProcessed[GLFW_KEY_F11] = true;
    }

    if (World->Mode == MENU)
    {
        if (IsKeyPressed(GLFW_KEY_ENTER) && !IsKeyProcessed(GLFW_KEY_ENTER))
        {
            World->Mode = GLOBAL;
            KeyboardInput.KeysProcessed[GLFW_KEY_ENTER] = true;
        }
    }


    else if (World->Mode == GLOBAL)
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

        if (IsKeyPressed(GLFW_KEY_ESCAPE) && !IsKeyProcessed(GLFW_KEY_ESCAPE))
        {
            SetUntargetableAll(World->Components.Targetables);
        }

        if (IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && !IsMouseButtonProcessed(GLFW_MOUSE_BUTTON_LEFT))
        {
            vec2 CursorWorldPos = GetCursorWorldPos(Window, Camera);
            TryToSetTargetable(World->Components.Targetables, World->Components.Colliders, CursorWorldPos);
        }

        if (IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT) && !IsMouseButtonProcessed(GLFW_MOUSE_BUTTON_RIGHT))
        {
            vec2 CursorWorldPos = GetCursorWorldPos(Window, Camera);

            if (CursorWorldPos.X >= 0 && CursorWorldPos.X < WORLD_WIDTH && CursorWorldPos.Y >= 0 && CursorWorldPos.Y < WORLD_HEIGHT)
            {
                ChangeTargetedDest(World->Components.Targetables, World->Components.Motions, World->Components.Units,
                                   World->Components.Transforms, World->Components.Colliders, CursorWorldPos);
            }
        }

        // // NOTE(insolence): Buildings' events
        if (IsKeyPressed(GLFW_KEY_B) && !IsKeyProcessed(GLFW_KEY_B))
        {
            StartUnitProduction(World->Components.Buildings, World->Components.Targetables);
            KeyboardInput.KeysProcessed[GLFW_KEY_B] = true;

        }
        if(IsKeyPressed(GLFW_KEY_H) && !IsKeyProcessed(GLFW_KEY_H))
        {
            StopUnitProduction(World->Components.Buildings);
            KeyboardInput.KeysProcessed[GLFW_KEY_H] = true;
        }
    }

    else if (World->Mode == BATTLE)
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

// bool
// CheckUnitCollisions(game_world *World, int UnitIndex)
// {
//     unit *Unit = &World->Player.Units[UnitIndex];

//     for (int UnitIndex = 0; UnitIndex < World->Player.UnitsNum; UnitIndex++)
//     {
//         if (Unit != &World->Player.Units[UnitIndex])
//         {
//             if (Intersect(World->Player.Units[UnitIndex].Collider, Unit->Collider))
//             {
//                 World->Player.Units[UnitIndex].TargetPos = World->Player.Units[UnitIndex].Pos;
//                 return true;
//             }
//         }
//     }

//     for (int EnemyIndex = 0; EnemyIndex < World->Objects.Enemies.size(); EnemyIndex++)
//     {
//         if (World->Objects.Enemies[EnemyIndex].Pos != NULL_POS && Intersect(Unit->Collider, World->Objects.Enemies[EnemyIndex].Collider))
//         {
//             battlefield NewBattlefield = CreateBattlefield(Unit->Pos);
//             World->ActiveBattlefields.push_back(NewBattlefield);
//             World->ActiveBattleNum = World->ActiveBattlefields.size() - 1;
//             World->Mode = BATTLE;
//             MusicSwitched = false;
//             NullifyEnemy(&World->Objects.Enemies[EnemyIndex]);
//             return true;
//         }
//     }
//     for (int ObstacleIndex = 0; ObstacleIndex < World->Objects.Obstacles.size(); ObstacleIndex++)
//     {
//         if (Intersect(Unit->Collider, World->Objects.Obstacles[ObstacleIndex].Collider))
//         {
//             return true;
//         }
//     }
//     for (int ChestIndex = 0; ChestIndex < World->Objects.Chests.size(); ChestIndex++)
//     {
//         if (World->Objects.Chests[ChestIndex].Pos != NULL_POS && Intersect(Unit->Collider, World->Objects.Chests[ChestIndex].Collider))
//         {
//             Unit->Power += World->Objects.Chests[ChestIndex].Value;
//             World->Objects.Chests[ChestIndex].Pos = NULL_POS;
//             return true;
//         }
//     }
//     for (int BuildingIndex = 0; BuildingIndex < World->Objects.Buildings.size(); BuildingIndex++)
//     {
//         if (Intersect(Unit->Collider, World->Objects.Buildings[BuildingIndex].Collider))
//         {
//             return true;
//         }
//     }
//     for (int ResourceIndex = 0; ResourceIndex < World->Objects.Resources.size(); ResourceIndex++)
//     {
//         if (World->Objects.Resources[ResourceIndex].Pos != NULL_POS && Intersect(Unit->Collider, World->Objects.Resources[ResourceIndex].Collider))
//         {
//             if (Unit->Type == WORKER)
//             {
//                 if (World->Objects.Resources[ResourceIndex].Type == STONE)
//                 {
//                     World->Player.Stone += World->Objects.Resources[ResourceIndex].Amount;
//                 }
//                 else if (World->Objects.Resources[ResourceIndex].Type == SAPPHIRE)
//                 {
//                     World->Player.Sapphires += World->Objects.Resources[ResourceIndex].Amount;
//                 }
//                 World->Objects.Resources[ResourceIndex].Pos = NULL_POS;
//             }
//             else
//             {
//                 return true;
//             }
//         }
//     }

//     return false;
// }

// NOTE(insolence): Range is from{-1,1} to{-1,1}
void
RenderGlobalMapGui(game_world *World)
{
    DrawTexturedRectangleOnScreen(vec2{-1.f, -1.f}, vec2{1.f, -0.4f}, GetTexture("mist.jpg"));

    int TargetedIndex = GetTargetedIndex(World->Components.Targetables);
    if (TargetedIndex != -1)
    {
        DrawRectangleOnScreen(vec2{-1.f, -1.f}, vec2{-0.6f, -0.4f}, SILVER);
        DrawTexturedRectangleOnScreen(vec2{-1.f, -1.f}, vec2{-0.6f, -0.4f}, GetTexture("frame.png"));
        DrawTexturedRectangleOnScreen(vec2{-0.9f, -0.9f}, vec2{-0.7f, -0.5f},  GetTexture(World->Components.Drawables[TargetedIndex].Texture));
    }

    // NOTE(insolence): Display player resources
    std::string PlayerStonesStr = std::to_string(World->Player.Stone);
    std::string PlayerSapphiresStr = std::to_string(World->Player.Sapphires);

    DrawTexturedRectangleOnScreen(vec2{0.72f, 0.9f}, vec2{0.79f, 0.97f}, GetTexture("stone_resource.png"));
    RenderTextOnScreen(PlayerStonesStr.c_str(), vec2{0.8f, 0.92f}, GOLD);

    DrawTexturedRectangleOnScreen(vec2{0.87f, 0.9f}, vec2{0.94f, 0.97f}, GetTexture("sapphire_resource.png"));
    RenderTextOnScreen(PlayerSapphiresStr.c_str(), vec2{0.95f, 0.92f}, GOLD);
}

void
RenderGlobalMap(game_world *World, orthographic_camera *Camera, std::vector<dir_light> DirLights, std::vector<point_light> PointLights, std::vector<spotlight_light> SpotLights)
{
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

    DisplayDrawables(World->Components.Transforms, World->Components.Drawables, World->Components.Targetables,
                     DirLights, PointLights, SpotLights, Camera);

    DisplayAttackerPowers(World->Components.Transforms, World->Components.Attackers, Camera);

    DisplayProductionTime(World->Components.Transforms, World->Components.Buildings, Camera);

    RenderGlobalMapGui(World);
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
                //DrawRectangleTextured(&ActiveBattle.BattleCamera, Transform(vec2{(float)X, (float)Y}, 0.f, 0.5f),
                 //                     GetTexture(World->Player.Units[World->Player.UnitChosen].Texture), GetNormalFromTexture(World->Player.Units[World->Player.UnitChosen].Texture),
                 //                     std::vector<dir_light>(), std::vector<point_light>(), std::vector<spotlight_light>());
            }
        }
    }

    RenderBattlefieldGui(&ActiveBattle);

    DrawParticles(&ActiveBattle.BattleCamera, Particles);
}

void
UpdateAndRender(GLFWwindow *Window, orthographic_camera *Camera)
{
    float DeltaTime = 0.f;
    float LastFrame = 0.f;

    irrklang::ISound *MainMusic;
    irrklang::ISound *BattleMusic;

    irrklang::ISoundEngine *SoundEngine = irrklang::createIrrKlangDevice();
    SoundEngine->setSoundVolume(0.08f);

    game_world World = {};
    World.Mode = MENU;

    World.Player.Stone = 100;
    World.Player.Sapphires = 30;

    for (int i = 0; i < 3; i++)
    {
        World.Entities.push_back(AddUnit(i, &World.Components, transform{vec2{(float)i, 0}, 1.f},
                                         motion{vec2{-1, -1}, vec2{0, 0}, 1}, "archer.png", ARCHER, 50, 35));
    }

    World.Entities.push_back(AddEnemy(3, &World.Components, transform{vec2{6, 3}, 1.f},
                             motion{ vec2{-1, -1}, vec2{0, 0}, 1}, "monster.png", 20, 40));


    World.Entities.push_back(AddBuilding(4, &World.Components, transform{vec2{5, 5}, 2.f}, "barracks.png", barracks, 10));

    for (int i = 5; i < 25; i++)
    {
        World.Entities.push_back(AddObstacle(i, &World.Components, transform{vec2{GetRandomFloat(2, 25), GetRandomFloat(2, 25)}, 1}, "rocks.png"));
    }

    for (int i = 25; i < 35; i++)
    {
        resource_type Type = i % 2 ? stone : sapphires;
        World.Entities.push_back(AddResource(i, &World.Components, transform{vec2{GetRandomFloat(1, 30), GetRandomFloat(3, 30)}, 1},
                                 (Type == stone ? "stone_minable.png" : "sapphire_minable.png"), Type, (Type == stone ? 10 : 5)));
    }

    for (int i = 36; i < 45; i++)
    {
        World.Entities.push_back(AddChest(i, &World.Components, transform{vec2{GetRandomFloat(4, 20),
                                 GetRandomFloat(8, 25)}, 1}, "treasure.png", 5));
    }

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

        if (World.Mode == GLOBAL)
        {
            if (!MusicSwitched)
            {
                SoundEngine->stopAllSounds();
                SoundEngine->play2D("W:/Insosure-Engine/assets/audio/onward.mp3", true);
                MusicSwitched = true;
            }

            RecalculateViewMatrix(Camera);

            bool UnitToBeProduced = UpdateBuildingsProduction(World.Components.Buildings, DeltaTime);
            if (UnitToBeProduced)
            {
                // World.Entities.push_back(AddUnit(46, &World.Components, transform{vec2{World->Objects.Buildings[i].Pos.X - 1.f, World->Objects.Buildings[i].Pos.Y - 1.f}}, 1.f},
                //                          motion{vec2{-1, -1}, vec2{0, 0}, 1}, "archer.png", ARCHER, 50, 35));
                World.Entities.push_back(AddUnit(46, &World.Components, transform{vec2{1.f, 1.f}, 1.f},
                                         motion{vec2{-1, -1}, vec2{0, 0}, 1}, "archer.png", ARCHER, 50, 35));
            }
            MoveEntities(World.Components.Transforms, World.Components.Motions, World.Components.Colliders, DeltaTime);

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
        else if (World.Mode == MENU)
        {
            DrawTexturedRectangleOnScreen(vec2{-1.f, -1.f}, vec2{1.f, 1.f}, GetTexture("intro.png"));
        }

        // NOTE(insolence): Displaying FPS and MSPerFrame, maybe change back to floats later?
        int Fps = 1.f/DeltaTime;
        int MsPerFrame = DeltaTime * 1000.f;
        std::string FpsStr = "FPS: " + std::to_string(Fps);
        std::string MSPerFrameStr = "MS per frame: " + std::to_string(MsPerFrame);
        RenderTextOnScreen(FpsStr.c_str(), vec2{-0.99f, 0.95f}, GOLD);
        RenderTextOnScreen(MSPerFrameStr.c_str(), vec2{-0.99f, 0.9f}, GOLD);


        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        Blur(&HdrFB, &PingpongFB[0]);

        glBindFramebuffer(GL_FRAMEBUFFER, PostprocessingFB.ID);
        ApplyHDR(HdrFB.TextureAttachment[0], PingpongFB[0].TextureAttachment[0], 1.0f);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        PostprocessScreenTexture(PostprocessingFB.TextureAttachment[0], Effects);

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
    //TextureCache = CreateHashMap<std::string, uint>();
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

    Effects.Inversion = false;
    Effects.Grayscale = false;
    Effects.Blur = false;

    UpdateAndRender(Window, &GlobalCamera);

    glfwTerminate();
}