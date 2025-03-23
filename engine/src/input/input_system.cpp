#include <GLFW/glfw3.h>

#include "input/input_system.h"

namespace DF::Input
{
    class GLFWInput : public InputSystem
    {
    private:
        std::map<int, Key> m_glfwKeyMap{
            { GLFW_KEY_ESCAPE, Key::ESC },
            { GLFW_KEY_ENTER,  Key::ENTER },
            { GLFW_KEY_SPACE,  Key::SPACE },
            { GLFW_KEY_N,      Key::N },
        };
        std::map<Key, std::vector<std::function<void()>>> m_keyCallbacksMap{};

    public:
        GLFWInput() = delete;

        explicit GLFWInput(const Render::Window* window) noexcept
        {
            GLFWwindow* glfwWindow{ static_cast<GLFWwindow*>(window->getRawWindow()) };

            glfwSetWindowUserPointer(glfwWindow, this);
            glfwSetKeyCallback(glfwWindow, glfwKeyCallback);
        }

        bool keyPressed(Key key) const override;

        bool mouseButtonPressed(MouseButton button) const override;

        void onKeyPress(Key key, std::function<void()>) override;

        void onMouseButtonPress(MouseButton button, std::function<void()>) override;

    private:
        Key getMappedKeyGLFW(int key) const;

        static void glfwKeyCallback(GLFWwindow* window, int key, int, int, int);
    };

    bool GLFWInput::keyPressed(Key key) const
    {
        return false;
    }

    bool GLFWInput::mouseButtonPressed(MouseButton button) const
    {
        return false;
    }

    void GLFWInput::onKeyPress(Key key, std::function<void()> callback)
    {
        m_keyCallbacksMap[key].push_back(std::move(callback));
    }

    void GLFWInput::onMouseButtonPress(MouseButton button, std::function<void()> callback)
    {

    }

    Key GLFWInput::getMappedKeyGLFW(int key) const
    {
        auto it{ m_glfwKeyMap.find(key) };

        if (it == m_glfwKeyMap.end()) return Key::UNDEFINED;

        return it->second;
    }

    void GLFWInput::glfwKeyCallback(GLFWwindow* window, int key, int, int action, int)
    {
        auto* self = static_cast<GLFWInput*>(glfwGetWindowUserPointer(window));

        if (self && action == GLFW_PRESS)
        {
            auto callbacks{ self->m_keyCallbacksMap[self->getMappedKeyGLFW(key)] };

            for (const auto& callback : callbacks)
            {
                callback();
            }

            std::cout << "Key pressed: " << key << '\n';
        }
    }

    //std::shared_ptr<GLFWInput> inputSystem{ std::make_shared<GLFWInput>() };

    //std::shared_ptr<InputSystem> getInputSystem()
    //{
    //    return inputSystem;
    //}

    std::unique_ptr<InputSystem> InputSystem::create(const Render::Window* window)
    {
        return std::make_unique<GLFWInput>(window);
    }
}
