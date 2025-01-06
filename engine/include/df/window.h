#pragma once

#include <string>
#include <iostream>

#include <GLFW/glfw3.h>

namespace DF
{
    class Window
    {
    public:
        explicit Window(int width, int height, std::string title);

        ~Window();

        bool init();

        operator bool() { return m_valid; }

        bool shouldClose() { return glfwWindowShouldClose(m_window); }

        void update();

    private:
        int m_width{ 800 };
        int m_height{ 600 };
        std::string m_title{ "Window" };
        bool m_valid{ false };
        GLFWwindow* m_window;
    };
}
