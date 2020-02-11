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
#include "math/perlin.h"
#include "utils/hash_map.cpp"
#include "utils/array_list.h"
#include "utils/time.h"

//#include <audio/audio.h>
#include "debug.h"
#include "renderer/texture.h"
#include "renderer/sprite.h"
#include "renderer/orthographic_camera.h"
#include "physics/physics.h"
#include "renderer/framebuffer.h"
#include "renderer/text.h"
#include "renderer/particle_system.h"
#include "renderer/renderer.cpp"
#include "window.h"

global_variable bool PlayerMoved = false;
global_variable vec2 OldPosition;
void
ProcessInput(GLFWwindow *Window, orthographic_camera *Camera, game_world *World, float Dt)
{
    // NOTE(insolence): Fullscreen
    if (glfwGetKey(Window, GLFW_KEY_F12) == GLFW_PRESS)
    {
        SwitchFullscreen(Window);
    }

    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(Window, true);
    }

    float CameraSpeed = 4.f;
    // NOTE(insolence): For faster camera movement press UP, used for debugging
    // if (glfwGetKey(Window, GLFW_KEY_UP) == GLFW_PRESS)
    // {
    //     CameraSpeed = 10.f;
    // }
    if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
    {
        Camera->Position.Y += CameraSpeed * Dt;
    }
    if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
    {
        Camera->Position.Y -= CameraSpeed * Dt;
    }
    if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
    {
        Camera->Position.X -= CameraSpeed * Dt;
    }
    if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
    {
        Camera->Position.X += CameraSpeed * Dt;
    }

    // if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
    // {
    //     Camera->Rotation += 1.f;
    // }
    // else if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
    // {
    //     Camera->Rotation -= 1.f;
    // }

    static bool UpProcessed    = false;
    static bool DownProcessed  = false;
    static bool LeftProcessed  = false;
    static bool RightProcessed = false;
    if (glfwGetKey(Window, GLFW_KEY_UP) == GLFW_PRESS && !UpProcessed)
    {
        OldPosition = World->PlayerPos;
        World->PlayerPos.Y++;
        UpProcessed = true;
        PlayerMoved = true;
    }
    else if (glfwGetKey(Window, GLFW_KEY_DOWN) == GLFW_PRESS && !DownProcessed)
    {
        OldPosition = World->PlayerPos;
        World->PlayerPos.Y--;
        DownProcessed = true;
        PlayerMoved = true;
    }
    if (glfwGetKey(Window, GLFW_KEY_LEFT) == GLFW_PRESS && !LeftProcessed)
    {
        OldPosition = World->PlayerPos;
        World->PlayerPos.X--;
        LeftProcessed = true;
        PlayerMoved = true;
    }
    else if (glfwGetKey(Window, GLFW_KEY_RIGHT) == GLFW_PRESS && !RightProcessed)
    {
        OldPosition = World->PlayerPos;
        World->PlayerPos.X++;
        RightProcessed = true;
        PlayerMoved = true;
    }

    if (glfwGetKey(Window, GLFW_KEY_UP) == GLFW_RELEASE)
    {
        UpProcessed = false;
    }
    if (glfwGetKey(Window, GLFW_KEY_DOWN) == GLFW_RELEASE)
    {
        DownProcessed = false;
    }
    if (glfwGetKey(Window, GLFW_KEY_LEFT) == GLFW_RELEASE)
    {
        LeftProcessed = false;
    }
    if (glfwGetKey(Window, GLFW_KEY_RIGHT) == GLFW_RELEASE)
    {
        RightProcessed = false;
    }
}


void
UpdateAndRender(GLFWwindow *Window, orthographic_camera *Camera, postprocessing_effects *Effects)
{
    float DeltaTime = 0.f;
    float LastFrame = 0.f;

    irrklang::ISoundEngine *SoundEngine = irrklang::createIrrKlangDevice();
    SoundEngine->setSoundVolume(0.5f);
    SoundEngine->play2D("W:/Insosure-Engine/assets/audio/onward.mp3", true);

    game_world World = {};
    for (int Y = 0; Y < 300; Y++)
    {
        for (int X = 0; X < 300; X++)
        {
            World.Tiles[X][Y] = rand() % 2;
        }
    }
    World.PlayerPos = vec2{0, 0};

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

        glBindFramebuffer(GL_FRAMEBUFFER, HdrFB.ID);

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.01f, 0.01f, 0.01f, 1.f);

        RecalculateViewMatrix(Camera);

        // NOTE(insolence): Actual drawing
        vec2 CurrentTilePos = { -Camera->Position.X, -Camera->Position.Y};
        mat4 CurrentTileTransform = Transform(CurrentTilePos, 0.f, 1.f);
        //printf("CurrentTilePos X: %.2f Y: %.2f \n", CurrentTilePos.X, CurrentTilePos.Y);

        for (int Y = CurrentTilePos.Y - 5; Y < CurrentTilePos.Y + 5; Y++)
        {
            for (int X = CurrentTilePos.X - 7; X < CurrentTilePos.X + 7; X++)
            {
                if (X < 0 || Y < 0 || X > 300 || Y > 300)
                    continue;

                int Value = World.Tiles[X][Y];
                if (Value == 0)
                {
                    DrawRectangleTextured(Camera, Transform(vec2{(float)X, (float)Y}, 0.f, 1.f), GetTexture("grass.jpg"));
                }
                else if (Value == 1)
                {
                    DrawRectangleTextured(Camera, Transform(vec2{(float)X, (float)Y}, 0.f, 1.f), GetTexture("rock.png"));
                }

                if (X == World.PlayerPos.X && Y == World.PlayerPos.Y)
                {
                    DrawRectangleTextured(Camera, Transform(vec2{(float)X, (float)Y}, 0.f, 1.f), GetTexture("roflanlitso.png"));
                }
            }
        }

        if (PlayerMoved)
        {
            for (int i = 0; i < 15; i++)
            {
                Particles.push_back(SpawnParticle(OldPosition, {0.f, 0.f}, {0.f, 0.f}));
                PlayerMoved = false;
            }
        }
        UpdateParticles(Particles, DeltaTime);
        DrawParticles(Camera, Particles,  GetTexture("star.png"));

        RenderText("Heroes Budget Version", 20.f, 200.f, 1.f, {4, 1, 1});
        RenderText("The legend has been born!!!", 20.f, 50.f, 1.f, {2, 4, 4});

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

    orthographic_camera Camera;
    float AspectRatio = 16.f / 9.f;
    float ZoomLevel = 2.f;
    SetViewProjection(&Camera, -ZoomLevel * AspectRatio, ZoomLevel * AspectRatio, -ZoomLevel, ZoomLevel); // NOTE(insolence): The ratio must be 16/9 in order to preserve the shapes

    postprocessing_effects Effects;
    Effects.Inversion = false;
    Effects.Grayscale = false;
    Effects.Blur = false;

    UpdateAndRender(Window, &Camera, &Effects);

    glfwTerminate();
}