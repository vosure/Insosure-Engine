#include "main.h"

// All includes
#include <glad/src/glad.c>
#include <glfw/include/glfw3.h>
#include <stb_image/stb_image.cpp>
#include <ft2build.h>
#include FT_FREETYPE_H  

#include "utils/file_utils.h"
#include "math/linear_math.h"
#include "math/math.h"
#include "math/perlin.h"
#include "utils/hash_map.cpp"
#include "utils/array_list.h"

#include "debug.h"
#include "renderer/texture.h"
#include "renderer/sprite.h"
#include "renderer/orthographic_camera.h"
#include "physics/physics.h"
#include "renderer/framebuffer.h"
#include "renderer/text.h"
#include "renderer/renderer.cpp"
#include "window.h"

void
ProcessInput(GLFWwindow *Window, orthographic_camera *Camera, float Dt)
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
    if (glfwGetKey(Window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        CameraSpeed = 10.f;
    }
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

    if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        Camera->Rotation += 1.f;
    }
    else if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
    {
        Camera->Rotation -= 1.f;
    }
}

void
UpdateAndRender(GLFWwindow *Window)
{
    float DeltaTime = 0.f;
    float LastFrame = 0.f;

    TextureCache = CreateHashMap();
    
    MakeFramebuffers(WINDOW_WIDTH, WINDOW_HEIGHT);
    MakeShaders();

    orthographic_camera Camera;
    float AspectRatio = 16.f / 9.f;
    float ZoomLevel = 2.6f;
    SetViewProjection(&Camera, -ZoomLevel * AspectRatio, ZoomLevel * AspectRatio, -ZoomLevel, ZoomLevel); // NOTE(insolence): The ratio must be 16/9 in order to preserve the shapes

    postprocessing_effects Effects;
    Effects.Inversion = false;
    Effects.Grayscale = false;
    Effects.Blur = false;

    // NOTE(insolence): Temp code for testing instancing
    mat4 Transforms[300] = {};
    for (int i = 0; i < ArrayCount(Transforms); i++)
    {
        Transforms[i] = Transform({(float)(i % 10), (float)(-i / 10)}, (i % 2 ? 45.f : -60.f), 1.f);
    }

    // NOTE(insolence): Main rendering loop
    while (!glfwWindowShouldClose(Window))
    {
        float CurrentFrame = glfwGetTime();
        DeltaTime = CurrentFrame - LastFrame;
        LastFrame = CurrentFrame;

        // printf("Seconds/frame: %.3f, ", DeltaTime);
        // printf("FPS: %.3f \n",  1.f/DeltaTime);

        ProcessInput(Window, &Camera, DeltaTime);

        glBindFramebuffer(GL_FRAMEBUFFER, HdrFB.ID);

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);

        RecalculateViewMatrix(&Camera);

        // // NOTE(insolence): Actual drawing

        DrawRectangle(&Camera, Transform({-2.f, 0.f}, 20.f, 2.f), {1.f, 3.f, 3.f});
        DrawRectangle(&Camera, Transform({-3.f, -3.f}, 0.f, 2.f), {5.f, 0.f, 0.f});

        DrawRectangleTextured(&Camera, Transform({-1.f, 3.f}, 15.f, 2.f), GetTexture("star.png"));
        DrawRectangleTextured(&Camera, Transform({1.f, 1.f},  0.f, 2.f),  GetTexture("test.jpg"));
        DrawRectangleTextured(&Camera, Transform({3.f, 3.f},  0.f, 2.f),  GetTexture("sun.jpg"));
        DrawRectangleTextured(&Camera, Transform({1.f, 1.f},  0.f, 2.f),  GetTexture("bush.png"));
        DrawRectangleTextured(&Camera, Transform({1.f, 1.f},  0.f, 2.f),  GetTexture("blending_transparent_window.png"));

        InstancedDrawRectangleTextured(&Camera, &Transforms[0], 300, GetTexture("star.png"));

        RenderText("Heroes Budget Version", 100.f, 500.f, 1.f, {4, 1, 1});
        RenderText("The legend has been born!!!", 100.f, 300.f, 1.f, {2, 4, 4});

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

    // NOTE(insolence): We cull back faces specified in CCW order,
    // Front faces are in CW order
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    UpdateAndRender(Window);

    glfwTerminate();
}