#include "window.h"

DF::Window::Window(int width, int height, std::string title)
    : m_width{ width }
    , m_height{ height }
    , m_title{ title }
    , m_valid{ false }
    , m_window{ nullptr }
{
}

DF::Window::~Window()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool DF::Window::init()
{
    std::cout << "Window initialization..." << '\n';

    m_valid = glfwInit();

    if (m_valid)
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);

        if (!m_window)
        {
            m_valid = false;

            return m_valid;
        }

        glfwMakeContextCurrent(m_window);
        /*glfwSetFramebufferSizeCallback(m_window, _onWindowResize);*/
    }

    std::cout << "Window initialization " << (m_valid ? "successfull" : "failed") << "!\n";

    return m_valid;
}

void DF::Window::update()
{
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}
