#include <stdio.h>

#include "glad.c"

#include "..\..\dependencies\glfw\include\glfw3.h"


int Start()
{
    printf("Testing the entry point to the engine!!!!!!");

    GLFWwindow *window;

    if (!glfwInit())
    {
        printf("Can't load glfw!");
        return -1;
    }

    window = glfwCreateWindow(1600, 1000, "Insosure Engine!!", NULL, NULL);
    if (!window)
    {
        printf("Failed to create a window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD!");
        return -1;
    }

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0, 1.0, 0.f, 1.f);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}