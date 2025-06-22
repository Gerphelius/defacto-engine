#include <GLFW/glfw3.h>
#include <fmt/format.h>

#include "render/window.hpp"
#include "input/input_system.hpp"

namespace DF::Render
{
    class WindowGLFW : public Window
    {
    private:
        struct GLFWwindowDeleter {
            void operator()(GLFWwindow* ptr) const {
                if (ptr) glfwDestroyWindow(ptr);
            }
        };

        int m_width{ 800 };
        int m_height{ 600 };
        std::string m_title{ "Window" };
        std::unique_ptr<GLFWwindow, GLFWwindowDeleter> m_window{};

    public:
        WindowGLFW() = delete;

        explicit WindowGLFW(int width, int height, std::string_view title);

        ~WindowGLFW() override { glfwTerminate(); }

        void update(float deltaTime) override;

        bool closed() const override;

        void* getRawWindow() const override { return m_window.get(); }

        static void onWindowResize(GLFWwindow* window, int width, int height);
    };

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
        glfwSetFramebufferSizeCallback(m_window.get(), onWindowResize);
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

    std::unique_ptr<Window> Window::create(int width, int height, std::string_view title)
    {
        return std::make_unique<WindowGLFW>(width, height, title);
    }

    void WindowGLFW::onWindowResize(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }
}
