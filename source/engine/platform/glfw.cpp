#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <gl/glew.h>

#include "platform.hpp"

namespace DF::PLATFORM
{
    WINDOW_CREATE(CreateWindow)
    {
        GLFWwindow* window{};

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
            // LOG: "Failed to create window."
        }

        if (glewInit() != GLEW_OK)
        {
            // LOG: "Failed to initialize glew"
        }

        glfwMakeContextCurrent(window);
        // glfwSetWindowUserPointer(window, ptr);

        glfwSwapInterval(0);

        // LOG: "Window initialization complete."

        return window;
    }

    WINDOW_UPDATE(UpdateWindow)
    {
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers((GLFWwindow*)window);
        glfwPollEvents();
    }

    WINDOW_CLOSED(WindowClosed)
    {
        return glfwWindowShouldClose((GLFWwindow*)window);
    }
};
