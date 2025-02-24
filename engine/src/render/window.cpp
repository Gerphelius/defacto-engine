#include "render/window.h"

namespace DF::Render
{
    Window::Window(int width, int height, std::string title)
        : m_width{ width }
        , m_height{ height }
        , m_title{ title }
        , m_valid{ false }
        , m_window{ nullptr }
    {
    }

    Window::~Window()
    {
        glfwTerminate();
    }

    bool Window::init()
    {
        std::cout << "Window initialization..." << '\n';

        m_valid = glfwInit();

        if (m_valid)
        {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            m_window = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>(
                glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr)
            );

            if (!m_window)
            {
                m_valid = false;

                return m_valid;
            }

            glfwMakeContextCurrent(m_window.get());
            /*glfwSetFramebufferSizeCallback(m_window, _onWindowResize);*/
        }

        std::cout << "Window initialization " << (m_valid ? "successful" : "failed") << "!\n";

        return m_valid;
    }

    void Window::update()
    {
        glfwSwapBuffers(m_window.get());
        glfwPollEvents();
    }
}
