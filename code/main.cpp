#include "main.h"

#include <vector>

// All includes
#include <glad/src/glad.c>
#include <glfw/include/glfw3.h>
#include <stb_image/stb_image.cpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <irrKlang/include/irrKlang.h>

#include "utils/file_utils.h"
#include "math/linear_math.h"
#include "math/math.h"
#include "random/random.h"
#include "math/perlin.h"
#include "utils/hash_map.cpp"
#include "utils/array_list.h"
#include "utils/time.h"

#include "debug.h"
#include "renderer/texture.h"
#include "renderer/sprite.h"
#include "renderer/orthographic_camera.h"
#include "physics/physics.h"
#include "renderer/framebuffer.h"
#include "renderer/directional_light.h"
#include "renderer/text.h"
#include "renderer/particle_system.h"
#include "renderer/renderer.cpp"
#include "input/input.h"
#include "window.h"

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

    if (IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(Window, true);
    }

    float CameraSpeed = 4.f;
    // NOTE(insolence): For faster camera movement press UP, used for debugging
    // if (glfwGetKey(Window, GLFW_KEY_UP) == GLFW_PRESS)
    // {
    //     CameraSpeed = 10.f;
    // }
    if (IsKeyPressed(GLFW_KEY_UP))
    {
        Camera->Position.Y -= CameraSpeed * Dt;
    }
    if (IsKeyPressed(GLFW_KEY_DOWN))
    {
        Camera->Position.Y += CameraSpeed * Dt;
    }
    if (IsKeyPressed(GLFW_KEY_LEFT))
    {
        Camera->Position.X += CameraSpeed * Dt;
    }
    if (IsKeyPressed(GLFW_KEY_RIGHT))
    {
        Camera->Position.X -= CameraSpeed * Dt;
    }

    // NOTE(insolence): Moving the camera with the mouse cursor
    float ScrollingSpeed = 1.f;
    vec2 MousePosition = GetMousePosition();
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

    // if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
    // {
    //     Camera->Rotation += 1.f;
    // }
    // else if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
    // {
    //     Camera->Rotation -= 1.f;
    // }

    if (IsKeyPressed(GLFW_KEY_W) && !IsKeyProcessed(GLFW_KEY_W))
    {
        World->Player.OldPos = World->Player.Pos;
        World->Player.Pos.Y++;
        PlayerMoved = true;
        KeyboardInput.KeysProcessed[GLFW_KEY_W] = true;
    }
    if (IsKeyPressed(GLFW_KEY_S) && !IsKeyProcessed(GLFW_KEY_S))
    {
        World->Player.OldPos = World->Player.Pos;
        World->Player.Pos.Y--;
        PlayerMoved = true;
        KeyboardInput.KeysProcessed[GLFW_KEY_S] = true;
    }
    else if (IsKeyPressed(GLFW_KEY_A) && !IsKeyProcessed(GLFW_KEY_A))
    {
        World->Player.OldPos = World->Player.Pos;
        World->Player.Pos.X--;
        PlayerMoved = true;
        KeyboardInput.KeysProcessed[GLFW_KEY_A] = true;
    }
    if (IsKeyPressed(GLFW_KEY_D) && !IsKeyProcessed(GLFW_KEY_D))
    {
        World->Player.OldPos = World->Player.Pos;
        World->Player.Pos.X++;
        PlayerMoved = true;
        KeyboardInput.KeysProcessed[GLFW_KEY_D] = true;
    }
}

// NOTE(insolence): Reveals 8 tiles around player
void
UpdateFogOfWar(game_world *World)
{
    for (int Y = World->Player.Pos.Y - 1; Y <= World->Player.Pos.Y + 1; Y++)
    {
        for (int X = World->Player.Pos.X - 1; X <= World->Player.Pos.X + 1; X++)
        {
            if (X < 0 || Y < 0 || X >= 300 || Y >= 300)
                continue;
            World->Tiles[X][Y].Visible = true;
        }
    }
}

void
UpdateAndRender(GLFWwindow *Window, orthographic_camera *Camera, postprocessing_effects *Effects)
{
    float DeltaTime = 0.f;
    float LastFrame = 0.f;

    irrklang::ISoundEngine *SoundEngine = irrklang::createIrrKlangDevice();
    SoundEngine->setSoundVolume(0.1f);
    //SoundEngine->play2D("W:/Insosure-Engine/assets/audio/onward.mp3", true);

    game_world World = {};
    for (int Y = 0; Y < WORLD_WIDTH; Y++)
    {
        for (int X = 0; X < WORLD_HEIGHT; X++)
        {
            World.Tiles[X][Y].Value = rand() % 2;
            World.Tiles[X][Y].Visible = true; // MAKE FALSE
        }
    }
    World.Tiles[4][3].Value = 2;
    World.Tiles[6][8].Value = 2;
    World.Tiles[5][1].Value = 2;
    World.Tiles[3][3].Value = 3;
    World.Tiles[10][8].Value = 3;
    World.Tiles[2][4].Value = 3;
    World.Tiles[1][3].Value = 4;
    World.Tiles[5][4].Value = 4;
    World.Tiles[2][4].Value = 4;
    World.Tiles[1][7].Value = 4;
    World.Tiles[5][2].Value = 4;

    World.Player.Pos = vec2{0, 0};
    World.Player.Power = 32;
    char PlayerPower[10];
    itoa(World.Player.Power, PlayerPower, 10); // Obtaining player power as a string

    std::vector<directional_light> Lights;
    Lights.push_back(DirLight({8, 7, 1}, {0, 0.9f, 0.01f}));
    Lights.push_back(DirLight({7, 9, 1}, {0.2, 0.1f, 0.9f}));
    Lights.push_back(DirLight({4, 4, 1}, {0.99f, 0.01f, 0.5f}));
    Lights.push_back(DirLight({1, 2, 1}, {0.01, 0.5f, 0.6f}));

    std::vector<particle> Particles(100);

    // NOTE(insolence): Main rendering loop
    while (!glfwWindowShouldClose(Window))
    {
        float CurrentFrame = glfwGetTime();
        DeltaTime = CurrentFrame - LastFrame;
        LastFrame = CurrentFrame;

        // printf("Seconds/frame: %.3f, ", DeltaTime);
        // printf("FPS: %.3f \n",  1.f/DeltaTime);

        ProcessInput(Window, Camera, &World, DeltaTime);

        UpdateFogOfWar(&World);

        glBindFramebuffer(GL_FRAMEBUFFER, HdrFB.ID);

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.01f, 0.01f, 0.01f, 1.f);

        RecalculateViewMatrix(Camera);

        // NOTE(insolence): Actual drawing
        vec2 CurrentTilePos = { -Camera->Position.X, -Camera->Position.Y};
        mat4 CurrentTileTransform = Transform(CurrentTilePos, 0.f, 1.f);

        for (int Y = CurrentTilePos.Y - 6; Y < CurrentTilePos.Y + 6; Y++)
        {
            for (int X = CurrentTilePos.X - 10; X < CurrentTilePos.X + 10; X++)
            {
                if (X < 0 || Y < 0 || X > WORLD_WIDTH || Y > WORLD_HEIGHT)
                    continue;

                int TileValue = World.Tiles[X][Y].Value;
                bool TileVisible = World.Tiles[X][Y].Visible;

                if ((TileValue == 0 || TileValue == 1) && TileVisible)
                {
                    DrawRectangleTextured(Camera, Transform(vec2{(float)X, (float)Y}, 0.f, 1.f), GetTexture("rock.png"), Lights);
                }
                else if (TileValue == 2 && TileVisible)
                {
                    DrawRectangleTextured(Camera, Transform(vec2{(float)X, (float)Y}, 0.f, 1.f), GetTexture("rock.png"), Lights);
                    DrawRectangleTextured(Camera, Transform(vec2{(float)X, (float)Y}, 0.f, 1.f), GetTexture("enemy.png"), Lights);
                }
                else if (TileValue == 3 && TileVisible)
                {
                    DrawRectangleTextured(Camera, Transform(vec2{(float)X, (float)Y}, 0.f, 1.f), GetTexture("rock.png"), Lights);
                    DrawRectangleTextured(Camera, Transform(vec2{(float)X, (float)Y}, 0.f, 1.f), GetTexture("enemy2.png"), Lights);
                }
                else if (TileValue == 4 && TileVisible)
                {
                    DrawRectangleTextured(Camera, Transform(vec2{(float)X, (float)Y}, 0.f, 1.f), GetTexture("rock.png"), Lights);
                    DrawRectangleTextured(Camera, Transform(vec2{(float)X, (float)Y}, 0.f, 1.f), GetTexture("treasure.png"), Lights);
                }
                else if (!TileVisible)
                {
                    DrawRectangle(Camera, Transform(vec2{(float)X, (float)Y}, 0.f, 1.f), {0, 0, 0});
                }

                if (X == World.Player.Pos.X && Y == World.Player.Pos.Y)
                {
                    DrawRectangleTextured(Camera, Transform(vec2{(float)X, (float)Y}, 0.f, 1.f), GetTexture("roflanface.png"), Lights);
                }
            }
        }

        if (PlayerMoved)
        {
            for (int i = 0; i < 15; i++)
            {
                Particles.push_back(SpawnParticle(World.Player.OldPos, 0.5f, {0.f, 0.f}, 2.5f, GetRandomFloat(0, 1) * 2.f));
                PlayerMoved = false;
            }
        }
        UpdateParticles(Particles, DeltaTime);
        DrawParticles(Camera, Particles,  GetTexture("star.png"));

        RenderTextOnScreen("The legend has been born!!!", 20.f, 50.f, 1.f, {2, 4, 4});

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
    // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    orthographic_camera Camera;
    Camera.Position = vec3{0, 0, 0}; //NOTE(vosure): check starting camera position
    float AspectRatio = 16.f / 9.f;
    float ZoomLevel = 4.f;
    SetViewProjection(&Camera, -ZoomLevel * AspectRatio, ZoomLevel * AspectRatio, -ZoomLevel, ZoomLevel); // NOTE(insolence): The ratio must be 16/9 in order to preserve the shapes

    postprocessing_effects Effects;
    Effects.Inversion = false;
    Effects.Grayscale = false;
    Effects.Blur = false;

    UpdateAndRender(Window, &Camera, &Effects);

    glfwTerminate();
}