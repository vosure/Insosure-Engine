#include "engine.h"

#include "utils/file_utils.h"
#include "math/linear_math.h"
#include "math/math.h"
#include "math/perlin.h"
#include "utils/hash_map.cpp"
#include "utils/array_list.h"

#include "renderer/texture.h"
#include "renderer/sprite.h"
#include "renderer/orthographic_camera.h"
#include "physics/physics.h"
#include "renderer/framebuffer.h"
#include "renderer/renderer.cpp"

global_variable framebuffer PostprocessingFB;
global_variable framebuffer HdrFB;
global_variable framebuffer PingpongFB[2]; // NOTE(insolence): Framebuffers for Gaussian blur

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__) 
void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

void FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height);

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

internal GLFWwindow*
SetUpWindow(int Width, int Height, char *WindowName, bool SetFullScreen)
{
    GLFWwindow *Window;

    if (!SetFullScreen)
    {
        Window = glfwCreateWindow(Width, Height, WindowName, NULL, NULL);
    }
    else
    {
        GLFWmonitor *Primary = glfwGetPrimaryMonitor();
        const GLFWvidmode *Mode = glfwGetVideoMode(Primary);

        Window = glfwCreateWindow(Width, Height, WindowName, Primary, NULL);	
        glfwSetWindowMonitor(Window, Primary, 0, 0, Mode->width, Mode->height, Mode->refreshRate);
        Width = Mode->width;
        Height = Mode->height;
    }

    if (!Window)
    {
        printf("Failed to create a window");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(Window);

    return Window;
}

internal void 
MakeFramebuffers(int Width, int Height)
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

internal void 
MakeShaders()
{
    Shader = CreateShader("shaders/basic.vert", "shaders/basic.frag");
    TexturedShader = CreateShader("shaders/texture.vert", "shaders/texture.frag");
    FBShader = CreateShader("shaders/postprocessing.vert", "shaders/postprocessing.frag");

    HDRShader = CreateShader("shaders/hdr.vert", "shaders/hdr.frag");
    glUseProgram(HDRShader.ShaderProgram);
    SetInt("Scene", HDRShader, 1);
    SetInt("BloomBlur", HDRShader, 0);
    glUseProgram(0);

    InstancedShader = CreateShader("shaders/instanced.vert", "shaders/instanced.frag");
    BlurShader = CreateShader("shaders/blur.vert", "shaders/blur.frag");
    TextShader = CreateShader("shaders/text.vert", "shaders/text.frag");
}

internal void 
DeleteShaders()
{
    DeleteShader(&Shader);
    DeleteShader(&TexturedShader);
    DeleteShader(&FBShader);
    DeleteShader(&HDRShader);
    DeleteShader(&InstancedShader);
    DeleteShader(&BlurShader);
    DeleteShader(&TextShader);
}

// NOTE(insolence): If fullscreen, then switch to windowed, and vice versa
internal void 
SwitchFullscreen(GLFWwindow *Window)
{
    glfwDestroyWindow(Window);

    if (IsFullscreen)
    {
        Window = SetUpWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Insosure Engine", false);
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        MakeFramebuffers(WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    else
    {
        // Make fullscreen
        Window = SetUpWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Insosure Engine", true);
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        MakeFramebuffers(SCREEN_WIDTH, SCREEN_HEIGHT);
        // Window = SetUpWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Insosure Engine", false);
        // glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        // MakeFramebuffers(WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    DeleteShaders();
    MakeShaders();
    UpdateTextureCache();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    IsFullscreen = !IsFullscreen;
}

void
ProcessInput(GLFWwindow *Window, orthographic_camera *Camera, float Dt)
{
    // NOTE(insolence): Fullscreen support
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

        RenderText("You are like commit, lol, anime commit", 100.f, 500.f, 1.f, {4, 1, 1});
        RenderText("Me: Commit commit commit", 100.f, 300.f, 1.f, {2, 4, 4});

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

void
LoadFreetype()
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        printf("Freetype error: Could not init FreeType Library \n");

    FT_Face face;
    if (FT_New_Face(ft, "../assets/fonts/arial.ttf", 0, &face))
        printf("Freetype error: Failed to load font");  

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // NOTE(insolence): Disable byte-alignment restriction

    for (GLubyte c = 0; c < 128; c++)
    {
        // NOTE(insolence): Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            printf("Freetype error: Failed to load Glyph \n");
            continue;
        }
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 
                     0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture, 
            vec2{ (float)face->glyph->bitmap.width, (float)face->glyph->bitmap.rows},
            vec2{ (float)face->glyph->bitmap_left,  (float)face->glyph->bitmap_top},
            (GLuint)face->glyph->advance.x 
            };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

int
Start()
{
    printf("The Engine has started!\n");

    if (!glfwInit())
    {
        printf("Can't load glfw!");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4.6);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4.6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    GLFWwindow *Window = SetUpWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Insosure Engine", false);
    if (!Window)
    {
        return BAD_RESULT;
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD!");
        return BAD_RESULT;
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

    GLFWmonitor *Primary = glfwGetPrimaryMonitor();
    const GLFWvidmode *Mode = glfwGetVideoMode(Primary);
    SCREEN_WIDTH = Mode->width;
    SCREEN_HEIGHT = Mode->height;

    LoadFreetype();

    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);

    // NOTE(insolence): We cull back faces specified in CCW order,
    // Front faces are in CW order
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    UpdateAndRender(Window);

    glfwTerminate();
    return SUCCESS;
}

GLenum 
glCheckError_(const char *file, int line)
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

void APIENTRY
glDebugOutput(GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei Length, const GLchar *Message, const void *UserParam)
{
    // NOTE(insolence): ignore non-significant error/warning codes
    if (ID == 131169 || ID == 131185 || ID == 131218 || ID == 131204) return; 

    printf("--------------- \n");
    printf("Debug message (%d): %s \n", ID, Message);

    switch (Source)
    {
        case GL_DEBUG_SOURCE_API:             printf("Source: API"); break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   printf("Source: Window System"); break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: printf("Source: Shader Compiler"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     printf("Source: Third Party"); break;
        case GL_DEBUG_SOURCE_APPLICATION:     printf("Source: Application"); break;
        case GL_DEBUG_SOURCE_OTHER:           printf("Source: Other"); break;
    } printf("\n");

    switch (Type)
    {
        case GL_DEBUG_TYPE_ERROR:               printf("Type: Error"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: printf("Type: Deprecated Behaviour"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  printf("Type: Undefined Behaviour"); break; 
        case GL_DEBUG_TYPE_PORTABILITY:         printf("Type: Portability"); break;
        case GL_DEBUG_TYPE_PERFORMANCE:         printf("Type: Performance"); break;
        case GL_DEBUG_TYPE_MARKER:              printf("Type: Marker"); break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          printf("Type: Push Group"); break;
        case GL_DEBUG_TYPE_POP_GROUP:           printf("Type: Pop Group"); break;
        case GL_DEBUG_TYPE_OTHER:               printf("Type: Other"); break;
    } printf("\n");
    
    switch (Severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         printf("Severity: high"); break;
        case GL_DEBUG_SEVERITY_MEDIUM:       printf("Severity: medium"); break;
        case GL_DEBUG_SEVERITY_LOW:          printf("Severity: low"); break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: printf("Severity: notification"); break;
    } printf("\n");
    printf("\n");
}


///// NOTE: Callbacks for OpenGL ///////////////

void
FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    glViewport(0, 0, Width, Height);
}
