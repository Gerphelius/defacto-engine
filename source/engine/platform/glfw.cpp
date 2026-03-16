#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <gl/glew.h>

#include "platform.hpp"

namespace DF::PLATFORM
{
struct WindowSize
{
    int width;
    int height;
};

void OnResize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// WindowSize GetWindowSize()
// {
//     return glfwGetWindowSize(window, &width, &height);
// }

WINDOW_CREATE(CreateWindow)
{
    GLFWwindow* window {};

    if (!glfwInit())
    {
        // LOG: "Failed to initialize window."
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!window)
    {
        std::cout << "Failed to create window.\n";

        glfwTerminate();
        // LOG: "Failed to create window."
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(window, OnResize);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize glew\n";
        // LOG: "Failed to initialize glew"
    }

    // glfwSetWindowUserPointer(window, ptr);

    // Disable depth testing for UI rendering so depth values do not affect
    // draw order. UI elements are rendered strictly in the order of glDraw calls.
    // glEnable(GL_DEPTH_TEST);

    DF::RENDER::Initialize();
    DF::UI::Initialize();

    // LOG: "Window initialization complete."

    return window;
}

float Map(float value, float low1, float high1, float low2, float high2)
{
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

WINDOW_UPDATE(UpdateWindow)
{
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int width, height;
    glfwGetWindowSize((GLFWwindow*)window, &width, &height);

    DF::UI::RenderUI((float)width, (float)height);

    glfwSwapBuffers((GLFWwindow*)window);
    glfwPollEvents();
}

WINDOW_CLOSED(WindowClosed)
{
    return glfwWindowShouldClose((GLFWwindow*)window);
}
};
