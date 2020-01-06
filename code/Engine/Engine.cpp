#include "engine.h"

#include <stdio.h>

#include "glad.c"
#include "..\..\dependencies\glfw\include\glfw3.h"

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

    *Window = glfwCreateWindow(1280, 720, "Insosure Engine", NULL, NULL);
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

// NOTE: Temp for testing
struct color
{
    float r, g, b;
};

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

    // NOTE: Main rendering loop
    while (!glfwWindowShouldClose(Window))
    {
        processInput(Window, &Color);

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(Color.r, Color.g, Color.b, 1.f);

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }
}

int 
Start()
{
    printf("The Engine has started!!");

    int WINDOW_WIDTH = 1280;
    int WINDOW_HEIGHT = 720;

    GLFWwindow *Window;
    if (!SetUpWindow(&Window, WINDOW_WIDTH, WINDOW_HEIGHT, "Insosure Engine"))
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
