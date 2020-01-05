#include <stdio.h>

#include "W:\Insosure-Engine\code\glfw\include\GLFW\glfw3.h"

int Start()
{
    printf("Testing the entry point to the engine!");

       GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
    {   
        printf("Can't load glfw!");
        return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        printf("Whatafuck");
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        //glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();


    return 0;
}