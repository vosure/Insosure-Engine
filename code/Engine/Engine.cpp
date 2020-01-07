#include "engine.h"

#include <stdio.h>

#include "..\..\dependencies\glad\src\glad.c"
#include "..\..\dependencies\glfw\include\glfw3.h"

#include "renderer/renderer.cpp"


void FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height);

internal int
SetUpWindow(GLFWwindow **Window, int Width, int Height, char* Name)
{
    if (!glfwInit())
    {
        printf("Can't load glfw!");
        return BAD_RESULT;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    *Window = glfwCreateWindow(Width, Height, Name, NULL, NULL);
    if (!*Window)
    {
        printf("Failed to create a window");
        glfwTerminate();
        return BAD_RESULT;
    }
    glfwMakeContextCurrent(*Window);

    return SUCCESS;
}

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

void processInput(GLFWwindow *Window, color *Color)
{
    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(Window, true);
    }

    if (glfwGetKey(Window, GLFW_KEY_UP))
    {
        Color->r += 0.05f;
    }
    if (glfwGetKey(Window, GLFW_KEY_DOWN))
    {
        Color->r -= 0.05f;
    }
}

void UpdateAndRender(GLFWwindow *Window)
{
    color Color = { 0.6f, 0.4f, 0.f };

    // NOTE(insolence): Main rendering loop
    while (!glfwWindowShouldClose(Window))
    {
        processInput(Window, &Color);

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(Color.r, Color.g, Color.b, 1.f);

        DrawRectangle(-0.5f, -0.5f, 0.5f, 0.5f, {0.1f, 0.8f, 1.f});

        float TexCoords[] = {
            -1.f, -1.f, 
             1.f,  1.f
        };
        texture Texture = CreateTexture("D:/dev/InsosureEngine/code/Engine/assets/textures/test.jpg", GL_NEAREST, GL_REPEAT); // TODO(insolence): change path to relative
        Texture.TexCoords = TexCoords;

        DrawRectangleTextured(-0.0f, -0.0f, 0.5f, 0.5f, Texture);

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }
}

int 
Start()
{
    printf("The Engine has started!");

    int WINDOW_WIDTH = 1280;
    int WINDOW_HEIGHT = 720;
    char* WindowName = "Insosure Engine";

    GLFWwindow* Window;
    if (!SetUpWindow(&Window, WINDOW_WIDTH, WINDOW_HEIGHT, WindowName))
    {
        return BAD_RESULT;
    }

    if (!LoadGlad())
    {
        printf("Failed to initialize GLAD!");
        return BAD_RESULT;
    }

    glViewport(0, 0, 1280, 720);

    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);

    UpdateAndRender(Window);

    glfwTerminate(); // TODO: Pass somewhere else or remove
    return SUCCESS;
}


///// NOTE: Callbacks for OpenGL /////////////// 

void 
FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    glViewport(0, 0, Width, Height);
} 
