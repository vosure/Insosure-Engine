#include "engine.h"

// All includes
#include <glad/src/glad.c>
#include <glfw/include/glfw3.h>
#include <stb_image/stb_image.cpp>

#include "utils/file_utils.h"
#include "math/linear_math.h"
#include "math/math.h"
#include "utils/hash_map.cpp"

#include "renderer/renderer.cpp"
#include "renderer/orthographic_camera.h"
#include "physics/physics.h"
#include "renderer/framebuffer.h"

// HACK(insolence): Temp solution
global_variable GLFWwindow *Window;


void FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height);

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  printf("INVALID_ENUM"); break;
            case GL_INVALID_VALUE:                 printf("INVALID_VALUE"); break;
            case GL_INVALID_OPERATION:             printf("INVALID_OPERATION"); break;
            case GL_OUT_OF_MEMORY:                 printf("OUT_OF_MEMORY"); break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: printf("INVALID_FRAMEBUFFER_OPERATION"); break;
        }
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

internal int
LoadGlad()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return BAD_RESULT;
    }
    else
    {
        return SUCCESS;
    }
}

//internal GLFWwindow*
void
SetUpWindowAndGlad(int Width, int Height, bool SetFullScreen)
{
    char *WindowName = "Insosure Engine";

    //GLFWwindow *Window;

    if (!glfwInit())
    {
        printf("Can't load glfw!");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (!SetFullScreen)
    {
        Window = glfwCreateWindow(Width, Height, WindowName, NULL, NULL);
    }
    else
    {
        GLFWmonitor *Primary = glfwGetPrimaryMonitor();
        const GLFWvidmode *Mode = glfwGetVideoMode(Primary);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        printf("ModeWidth: %d, ModeHeight: %d \n", Mode->width, Mode->height);

        Window = glfwCreateWindow(Width, Height, WindowName, Primary, NULL);	
        glfwSetWindowMonitor(Window, Primary, 0, 0, Mode->width, Mode->height, Mode->refreshRate);
        Width = Mode->width;
        Height = Mode->height;
    }

    if (!Window)
    {
        printf("Failed to create a window");
        glfwTerminate();
        //return Window;
    }
    glfwMakeContextCurrent(Window);

    if (!LoadGlad())
    {
        printf("Failed to initialize GLAD!");
        //return Window;
    }
    glViewport(0, 0, Width, Height);

    CurrentWidth = Width;
    CurrentHeight = Height;

    printf("CurrenWidth: %d, CurrentHeight: %d \n", CurrentWidth, CurrentHeight);

    //return Window;
}

internal void
DestroyWindow()//(GLFWwindow *Window)
{
    if (Window)
        glfwDestroyWindow(Window);
}

texture BrickWall;
texture Bush; // GL_CLAMP_TO_EDGE for alpha textures
texture Sun;
texture SemiTranspWindow;
texture Star;

void
MakeTextures()
{
    if (BrickWall.ID)
    {
        glDeleteTextures(1, &BrickWall.ID);
        glDeleteTextures(1, &Bush.ID);
        glDeleteTextures(1, &Sun.ID);
        glDeleteTextures(1, &SemiTranspWindow.ID);
        glDeleteTextures(1, &Star.ID);
    }

    BrickWall = CreateTexture("test.jpg", GL_NEAREST, GL_REPEAT);
    Bush = CreateTexture("bush.png", GL_NEAREST, GL_CLAMP_TO_BORDER); // GL_CLAMP_TO_EDGE for alpha textures
    Sun = CreateTexture("sun.jpg", GL_NEAREST, GL_CLAMP_TO_BORDER);
    SemiTranspWindow = CreateTexture("blending_transparent_window.png", GL_NEAREST, GL_CLAMP_TO_BORDER);
    Star = CreateTexture("star.png", GL_NEAREST, GL_CLAMP_TO_BORDER);
}

framebuffer PostprocessingFB;
framebuffer HdrFB;
framebuffer PingpongFB[2]; //NOTE(insolence): Framebuffers for Gaussian blur

void MakeFramebuffers(int Width, int Height)
{
    if (PostprocessingFB.ID)
    {
        glDeleteFramebuffers(1, &PostprocessingFB.ID);
        glDeleteFramebuffers(1, &HdrFB.ID);
        glDeleteFramebuffers(1, &PingpongFB[0].ID);
        glDeleteFramebuffers(1, &PingpongFB[1].ID);
    }

    PostprocessingFB = CreateFramebuffer(Width, Height, GL_RGBA, 1, MAKE_RENDERBUFFER);
    HdrFB = CreateFramebuffer(Width, Height, GL_RGB16F, 2, MAKE_RENDERBUFFER);
    for (int i = 0; i < 2; i++)
    {
        PingpongFB[i] = CreateFramebuffer(Width, Height, GL_RGB16F, 1, RENDERBUFFER_NEEDLESS);
    }
}

void
ProcessInput(orthographic_camera *Camera, float Dt)
{
    // NOTE(insolence): Fullscreen support
    if (glfwGetKey(Window, GLFW_KEY_F12) == GLFW_PRESS)
    {
        if (IsFullscreen)
        {
            DestroyWindow();

            Window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "InsosureEngine", NULL, NULL);
            if (!Window)
            {
                glfwTerminate();
                printf("Window hasn't been initialized!");
                return;
            }
            glfwMakeContextCurrent(Window);
            glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

            MakeFramebuffers(WINDOW_WIDTH, WINDOW_HEIGHT);
            MakeTextures();

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            // Make fullscreen
            DestroyWindow();

            GLFWmonitor *Primary = glfwGetPrimaryMonitor();
            const GLFWvidmode *Mode = glfwGetVideoMode(Primary);

            Window = glfwCreateWindow(Mode->width, Mode->height, "InsosureEngine", Primary, NULL);
            if (!Window)
            {
                glfwTerminate();
                printf("Window hasn't been initialized!");
                return;
            }	
            glfwMakeContextCurrent(Window);
            glViewport(0, 0, Mode->width, Mode->height);

            MakeFramebuffers(Mode->width, Mode->height);
            MakeTextures();

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        IsFullscreen = !IsFullscreen;
        return;
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

    // NOTE(insolence): Camera rotation, yet to be done
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
Blur(framebuffer *HdrFB, framebuffer *PingpongFB)
{
    //local_persist 
    shader BlurShader = {};
    if (!BlurShader.ShaderProgram)
    {
        BlurShader = CreateShader("shaders/blur.vert", "shaders/blur.frag");
    }

    // NOTE(insolence): Blur bright fragments with 2-pass Gaussian blur
    int Horizontal = 1;
    bool FirstIteration = true;
    int Amount = 20;
    for (int i = 0; i < Amount; i++)
    {
        glUseProgram(BlurShader.ShaderProgram);
        glBindFramebuffer(GL_FRAMEBUFFER, PingpongFB[Horizontal].ID);
        SetInt("Image", BlurShader, 0);
        SetInt("Horizontal", BlurShader, Horizontal);
        glBindTexture(GL_TEXTURE_2D, FirstIteration ? HdrFB->TextureAttachment[1] : PingpongFB[!Horizontal].TextureAttachment[0]);
        RenderScreenTexture();
        Horizontal = !Horizontal;
        if (FirstIteration)
            FirstIteration = false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// NOTE(insolence): Temp, probably make a separate struct Transform
mat4
Transform(vec2 Pos, float Rotation, float ScaleAm)
{
    mat4 Result = Scale(ScaleAm) * Rotate(Rotation, {0, 0, 1}) * Translate({Pos.X, Pos.Y, 0});

    return Result;
}

void
UpdateAndRender(GLFWwindow *Window)
{
    float DeltaTime = 0.f;
    float LastFrame = 0.f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // NOTE(insolence): We cull back faces specified in CCW order,
    // Front faces are in CW order
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CW);

    MakeFramebuffers(WINDOW_WIDTH, WINDOW_HEIGHT);
    MakeTextures();

    orthographic_camera Camera;
    float AspectRatio = 16.f / 9.f;
    float ZoomLevel = 2.6f;
    SetViewProjection(&Camera, -ZoomLevel * AspectRatio, ZoomLevel * AspectRatio, -ZoomLevel, ZoomLevel); // NOTE(insolence): The ratio must be 16/9 in order to preserve the shapes

    postprocessing_effects Effects;
    Effects.Inversion = true;
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

        ProcessInput(&Camera, DeltaTime);

        glBindFramebuffer(GL_FRAMEBUFFER, HdrFB.ID);

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);

        RecalculateViewMatrix(&Camera);

        // // NOTE(insolence): Actual drawing
        DrawRectangle(&Camera, Transform({-2.f, 0.f}, 20.f, 2.f), {1.f, 3.f, 3.f});
        DrawRectangle(&Camera, Transform({-3.f, -3.f}, 0.f, 2.f), {5.f, 0.f, 0.f});

        DrawRectangleTextured(&Camera, Transform({3.f, 3.f}, 0.f, 2.f), Sun);
        DrawRectangleTextured(&Camera, Transform({1.f, 1.f}, 0.f, 2.f), BrickWall);
        DrawRectangleTextured(&Camera, Transform({1.f, 1.f}, 0.f, 2.f), Bush);
        DrawRectangleTextured(&Camera, Transform({1.f, 1.f}, 0.f, 2.f), SemiTranspWindow);
        DrawRectangleTextured(&Camera, Transform({-1.f, 3.f}, 15.f, 2.f), Bush);

        InstancedDrawRectangleTextured(&Camera, &Transforms[0], 300, Star);

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

int
Start()
{
    printf("The Engine has started!\n");

    //GLFWwindow *Window = SetUpWindowAndGlad(WINDOW_WIDTH, WINDOW_HEIGHT, false);
    SetUpWindowAndGlad(WINDOW_WIDTH, WINDOW_HEIGHT, false);
    if (!Window)
    {
        return BAD_RESULT;
    }

    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);

    UpdateAndRender(Window);

    printf("Does it get there?");

    glfwTerminate(); // TODO: Pass somewhere else or remove
    return SUCCESS;
}



///// NOTE: Callbacks for OpenGL ///////////////

void
FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    glViewport(0, 0, Width, Height);
}
