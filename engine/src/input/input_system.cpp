#include <unordered_map>
#include <vector>
#include <unordered_set>

#include <GLFW/glfw3.h>

#include "input/input_system.h"

namespace DF::Input
{
    class GLFWInput : public InputSystem
    {
    private:
        std::unordered_map<int, Key> m_glfwKeyMap{
            { GLFW_KEY_ESCAPE,      Key::ESC },
            { GLFW_KEY_ENTER,       Key::ENTER },
            { GLFW_KEY_SPACE,       Key::SPACE },
            { GLFW_KEY_SPACE,       Key::SPACE },
            { GLFW_KEY_LEFT_SHIFT,  Key::SHIFT_L },
            { GLFW_KEY_N,           Key::N },
            { GLFW_KEY_W,           Key::W },
            { GLFW_KEY_A,           Key::A },
            { GLFW_KEY_S,           Key::S },
            { GLFW_KEY_D,           Key::D },
        };
        std::unordered_map<Key, std::unordered_map<KeyEvent, std::vector<std::function<void()>>>> m_keyCallbacksMap{};
        std::unordered_set<Key> pressedKeys{};

    public:
        GLFWInput() = delete;

        explicit GLFWInput(const Render::Window* window) noexcept
        {
            GLFWwindow* glfwWindow{ static_cast<GLFWwindow*>(window->getRawWindow()) };

            glfwSetWindowUserPointer(glfwWindow, this);
            glfwSetKeyCallback(glfwWindow, glfwKeyCallback);
            glfwSetCursorPosCallback(glfwWindow, glfwMouseCallback);
            glfwSetScrollCallback(glfwWindow, glfwWheelCallback);
        }

        bool keyPressed(Key key) const override;

        bool mouseButtonPressed(MouseButton button) const override;

        void onKeyPress(Key key, KeyEvent event, std::function<void()>) override;

        void onMouseButtonPress(MouseButton button, std::function<void()>) override;

        void update() override;

    private:
        Key getMappedKeyGLFW(int key) const;

        static void glfwKeyCallback(GLFWwindow* window, int key, int, int, int);

        static void glfwMouseCallback(GLFWwindow* window, double xpos, double ypos);

        static void glfwWheelCallback(GLFWwindow* window, double xoffset, double yoffset);
    };

    bool GLFWInput::keyPressed(Key key) const
    {
        return false;
    }

    bool GLFWInput::mouseButtonPressed(MouseButton button) const
    {
        return false;
    }

    void GLFWInput::onKeyPress(Key key, KeyEvent event, std::function<void()> callback)
    {
        m_keyCallbacksMap[key][event].push_back(std::move(callback));
    }

    void GLFWInput::onMouseButtonPress(MouseButton button, std::function<void()> callback)
    {

    }

    void GLFWInput::update()
    {
        for (Key key : pressedKeys)
        {
            auto callbacks{ m_keyCallbacksMap[key][KeyEvent::HOLD] };

            for (const auto& callback : callbacks)
            {
                callback();
            }
        }
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

        if (!self || action == GLFW_REPEAT) return;

        KeyEvent keyEvent{};
        auto dfKey{ self->getMappedKeyGLFW(key) };

        switch (action)
        {
        case GLFW_PRESS:
            keyEvent = KeyEvent::PRESS;
            self->pressedKeys.insert(dfKey);
            break;
        case GLFW_RELEASE:
            keyEvent = KeyEvent::RELEASE;
            self->pressedKeys.erase(dfKey);
            break;
        }

        auto callbacks{ self->m_keyCallbacksMap[dfKey][keyEvent] };

        for (const auto& callback : callbacks)
        {
            callback();
        }

        std::cout
            << "Key "
            << (keyEvent == KeyEvent::PRESS ? "pressed:\t" : "released:\t")
            << static_cast<char>(key)
            << '\n';
    }

    void GLFWInput::glfwMouseCallback(GLFWwindow* window, double xpos, double ypos)
    {
        std::cout << "Mouse pos: (x " << xpos << ", y " << ypos << ")\n";
    }

    void GLFWInput::glfwWheelCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        std::cout << "Mouse wheel: (x " << xoffset << ", y " << yoffset << ")\n";
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
