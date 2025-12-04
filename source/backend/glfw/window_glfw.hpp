#pragma once

#include <string>
#include <memory>

#include "render/window.hpp"
#include "input_glfw.hpp"

namespace DF::Core
{
    class BackendFactory;
}

namespace DF::Backend
{
    class WindowGLFW : public DF::Render::Window
    {
    private:
        friend class DF::Core::BackendFactory;

        struct GLFWwindowDeleter {
            void operator()(GLFWwindow* ptr) const {
                if (ptr) glfwDestroyWindow(ptr);
            }
        };

        int m_width{ 800 };
        int m_height{ 600 };

        std::string m_title{ "Window" };
        std::unique_ptr<GLFWwindow, GLFWwindowDeleter> m_window{};
        InputGLFW* m_input{};
        ResizeCallback m_resizeCallback{};

    public:
        WindowGLFW() = delete;

        explicit WindowGLFW(int width, int height, std::string_view title);

        ~WindowGLFW() override { glfwTerminate(); }

        void update(float deltaTime) override;

        bool closed() const override;

        float getAspectRatio() const override { return static_cast<float>(m_width) / m_height; };

        void* getRawWindow() const override { return m_window.get(); }

        void setResizeCallback(ResizeCallback callback);

    private:
        void connectInput(InputGLFW* input);

        static void glfwResize(GLFWwindow* window, int width, int height);

        static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

        static void glfwMouseCallback(GLFWwindow* window, int key, int action, int mode);

        static void glfwCursorCallback(GLFWwindow* window, double xpos, double ypos);

        static void glfwWheelCallback(GLFWwindow* window, double xoffset, double yoffset);
    };
}
