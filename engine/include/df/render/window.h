#pragma once

#include <string>
#include <iostream>
#include <memory>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "input/input_manager.h"

namespace DF::Render
{
    // Custom deleter for GLFWwindow
    struct GLFWwindowDeleter {
        void operator()(GLFWwindow* ptr) const {
            if (ptr) glfwDestroyWindow(ptr);
        }
    };

    class Window
    {
    public:
        explicit Window(int width, int height, std::string title);

        ~Window();

        bool init();

        operator bool() { return m_valid; }

        bool shouldClose() { return glfwWindowShouldClose(m_window.get()); }

        void update();

        GLFWwindow* getWindow() const { return m_window.get(); }

    private:
        int m_width{ 800 };
        int m_height{ 600 };
        std::string m_title{ "Window" };
        bool m_valid{ false };
        std::unique_ptr<GLFWwindow, GLFWwindowDeleter> m_window{};
    };
}
