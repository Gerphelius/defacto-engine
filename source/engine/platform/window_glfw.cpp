#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <gl/glew.h>

#include "platform_window.hpp"

namespace DF::PLATFORM
{

Window CreateWindow(int width, int height, const char* title)
{
    GLFWwindow* glfwWindow {};

    if (!glfwInit())
    {
        std::cout << "Failed to initialize glfw window.\n";
    }

    // glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!glfwWindow)
    {
        std::cout << "Failed to create glfw window.\n";

        glfwTerminate();
    }

    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(0);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize glew. Call CreateWindow first.\n";
    }

    // glfwSetWindowUserPointer(window, ptr);

    std::cout << "Window initialization complete.\n";

    return Window { glfwWindow };
}

bool WindowClosed(Window* window)
{
    return glfwWindowShouldClose((GLFWwindow*)window->handle);
}

void SwapBuffers(Window* window)
{
    glfwSwapBuffers((GLFWwindow*)window->handle);
    glfwPollEvents();
}

Size GetFramebufferSize(Window* window)
{
    int width, height;
    glfwGetFramebufferSize((GLFWwindow*)window->handle, &width, &height);

    return Size(width, height);
}

}; // namespace DF::PLATFORM
