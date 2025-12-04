#include <iostream>

#include <GLFW/glfw3.h>
#include <fmt/format.h>

#include "window_glfw.hpp"

namespace DF::Backend
{
    WindowGLFW::WindowGLFW(int width, int height, std::string_view title)
        : m_width{ width }
        , m_height{ height }
        , m_title{ title }
        , m_window{ nullptr }
    {
        std::cout << "Window initialization..." << '\n';

        if (!glfwInit())
        {
            throw std::runtime_error{ "Failed to initialize window." };
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_window = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>(
            glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr)
        );

        if (!m_window)
        {
            throw std::runtime_error{ "Failed to create window." };
        }

        glfwMakeContextCurrent(m_window.get());
        glfwSetWindowUserPointer(m_window.get(), this);

        glfwSetFramebufferSizeCallback(m_window.get(), glfwResize);
        glfwSetKeyCallback(m_window.get(), glfwKeyCallback);
        glfwSetMouseButtonCallback(m_window.get(), glfwMouseCallback);
        glfwSetCursorPosCallback(m_window.get(), glfwCursorCallback);
        glfwSetScrollCallback(m_window.get(), glfwWheelCallback);

        glfwSwapInterval(0);

        std::cout << "Window initialization complete." << '\n';
    }

    void WindowGLFW::update(float deltaTime)
    {
        glfwPollEvents();
        glfwSwapBuffers(m_window.get());
        glfwSetWindowTitle(m_window.get(), fmt::format("{} | FPS: {}.", m_title, 1 / deltaTime).c_str());
    }

    bool WindowGLFW::closed() const
    {
        return glfwWindowShouldClose(m_window.get());
    }

    void WindowGLFW::setResizeCallback(ResizeCallback callback)
    {
        m_resizeCallback = callback;
    }

    void WindowGLFW::connectInput(InputGLFW* input)
    {
        m_input = input;
    }

    void WindowGLFW::glfwResize(GLFWwindow* window, int width, int height)
    {
        auto* self = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));

        if (self)
        {
            self->m_width = width;
            self->m_height = height;

            if (self->m_resizeCallback)
            {
                self->m_resizeCallback(static_cast<float>(width), static_cast<float>(height));
            }
        }

        glViewport(0, 0, width, height);
    }

    void WindowGLFW::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        auto* self = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));

        if (self && self->m_input)
        {
            self->m_input->keyCallback(key, action);
        }
    }

    void WindowGLFW::glfwMouseCallback(GLFWwindow* window, int key, int action, int mode)
    {
        auto* self = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));

        if (self && self->m_input)
        {
            self->m_input->mouseKeyCallback(key, action);
        }
    }

    void WindowGLFW::glfwCursorCallback(GLFWwindow* window, double xpos, double ypos)
    {
        auto* self = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));

        if (self && self->m_input)
        {
            self->m_input->cursorCallback(xpos, ypos);
        }
    }

    void WindowGLFW::glfwWheelCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        auto* self = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));

        if (self && self->m_input)
        {
            self->m_input->wheelCallback(xoffset, yoffset);
        }
    }
}
