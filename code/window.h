#pragma once

internal GLFWwindow*
SetUpWindow(int Width, int Height, char *WindowName, bool SetFullScreen)
{
    GLFWwindow *Window;

    if (!SetFullScreen)
    {
        Window = glfwCreateWindow(Width, Height, WindowName, NULL, NULL);
        glfwSetWindowPos(Window, 70, 70);
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
    SetInputCallbacks(Window);
    CurrentWidth = Width;
    CurrentHeight = Height;

    glfwSetCursorPos(Window, CurrentWidth/2, CurrentHeight/2);

    return Window;
}

// NOTE(insolence): If fullscreen, then switch to windowed, and vice versa
internal void
SwitchFullscreen(GLFWwindow *Window)
{
    glfwDestroyWindow(Window);

    if (IsFullscreen)
    {
        Window = SetUpWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Heroes Budget Version", false);
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        MakeFramebuffers(WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    else
    {
        // Make fullscreen
        Window = SetUpWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Heroes Budget Version", true);
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        MakeFramebuffers(SCREEN_WIDTH, SCREEN_HEIGHT);
        // Window = SetUpWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Insosure Engine", false);
        // glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        // MakeFramebuffers(WINDOW_WIDTH, WINDOW_HEIGHT);
        CurrentWidth = SCREEN_WIDTH;
        CurrentHeight = SCREEN_HEIGHT;
    }
    IsFullscreen = !IsFullscreen;

    MakeShaders();
    UpdateTextureCache();
    LoadFreetype();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

vec2 GetCursorPos(GLFWwindow *Window)
{
    vec2 CursorPos;

    double X, Y;
    glfwGetCursorPos(Window, &X, &Y);
    CursorPos.X = (float)X;
    CursorPos.Y = (float)Y;

    return CursorPos;
}