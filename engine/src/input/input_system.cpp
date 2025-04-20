#include <unordered_map>
#include <vector>
#include <unordered_set>

#include <GLFW/glfw3.h>

#include "input/input_system.hpp"

namespace DF::Input
{
    class GLFWInput : public InputSystem
    {
    private:
        using EventCallbacksMap = std::unordered_map<KeyEvent, std::vector<KeyPressCallback>>;

        std::unordered_map<int, Key> m_glfwKeyMap{
            { GLFW_KEY_ESCAPE,      Key::ESC },
            { GLFW_KEY_ENTER,       Key::ENTER },
            { GLFW_KEY_SPACE,       Key::SPACE },
            { GLFW_KEY_LEFT_SHIFT,  Key::SHIFT_L },
            { GLFW_KEY_N,           Key::N },
            { GLFW_KEY_W,           Key::W },
            { GLFW_KEY_A,           Key::A },
            { GLFW_KEY_S,           Key::S },
            { GLFW_KEY_D,           Key::D },
            { GLFW_KEY_Q,           Key::Q },
            { GLFW_KEY_E,           Key::E },
            { GLFW_KEY_UP,          Key::UP },
            { GLFW_KEY_DOWN,        Key::DOWN },
            { GLFW_KEY_LEFT,        Key::LEFT },
            { GLFW_KEY_RIGHT,       Key::RIGHT },
        };
        std::unordered_map<Key, EventCallbacksMap> m_keyCallbacksMap{};
        std::unordered_set<Key> m_pressedKeys{};
        std::unordered_set<MouseKey> m_pressedMouseKeys{};
        std::vector<MouseMoveCallback> m_mouseMoveCallbacks{};

    public:
        GLFWInput() = delete;

        explicit GLFWInput(const Render::Window* window) noexcept
        {
            GLFWwindow* glfwWindow{ static_cast<GLFWwindow*>(window->getRawWindow()) };

            glfwSetWindowUserPointer(glfwWindow, this);
            glfwSetKeyCallback(glfwWindow, glfwKeyCallback);
            glfwSetMouseButtonCallback(glfwWindow, glfwMouseCallback);
            glfwSetCursorPosCallback(glfwWindow, glfwCursorCallback);
            glfwSetScrollCallback(glfwWindow, glfwWheelCallback);
        }

        bool keyPressed(Key key) const override;

        bool mouseKeyPressed(MouseKey key) const override;

        void onKeyPress(Key key, KeyEvent event, KeyPressCallback) override;

        void onMouseKeyPress(MouseKey key, KeyPressCallback) override;

        void onMouseMove(MouseMoveCallback callback) override;

        void update() override;

    private:
        Key getMappedKeyGLFW(int key) const;

        static void glfwKeyCallback(GLFWwindow* window, int key, int, int, int);

        static void glfwMouseCallback(GLFWwindow* window, int button, int action, int);

        static void glfwCursorCallback(GLFWwindow* window, double xpos, double ypos);

        static void glfwWheelCallback(GLFWwindow* window, double xoffset, double yoffset);
    };

    bool GLFWInput::keyPressed(Key key) const
    {
        return false;
    }

    bool GLFWInput::mouseKeyPressed(MouseKey key) const
    {
        return m_pressedMouseKeys.contains(key);
    }

    void GLFWInput::onKeyPress(Key key, KeyEvent event, KeyPressCallback callback)
    {
        m_keyCallbacksMap[key][event].push_back(std::move(callback));
    }

    void GLFWInput::onMouseKeyPress(MouseKey button, KeyPressCallback callback)
    {
        
    }

    void GLFWInput::onMouseMove(MouseMoveCallback callback)
    {
        m_mouseMoveCallbacks.push_back(callback);
    }

    void GLFWInput::update()
    {
        for (Key key : m_pressedKeys)
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
            self->m_pressedKeys.insert(dfKey);
            break;
        case GLFW_RELEASE:
            keyEvent = KeyEvent::RELEASE;
            self->m_pressedKeys.erase(dfKey);
            break;
        }

        auto callbacks{ self->m_keyCallbacksMap[dfKey][keyEvent] };

        for (const auto& callback : callbacks)
        {
            callback();
        }
    }

    void GLFWInput::glfwMouseCallback(GLFWwindow* window, int button, int action, int)
    {
        auto* self = static_cast<GLFWInput*>(glfwGetWindowUserPointer(window));

        if (!self) return;

        MouseKey dfMouseKey{};

        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            dfMouseKey = MouseKey::LEFT;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            dfMouseKey = MouseKey::RIGHT;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            dfMouseKey = MouseKey::MIDDLE;
            break;
        }

        switch (action)
        {
        case GLFW_PRESS:
            self->m_pressedMouseKeys.insert(dfMouseKey);
            break;
        case GLFW_RELEASE:
            self->m_pressedMouseKeys.erase(dfMouseKey);
            break;
        }
    }

    void GLFWInput::glfwCursorCallback(GLFWwindow* window, double xpos, double ypos)
    {
        auto* self = static_cast<GLFWInput*>(glfwGetWindowUserPointer(window));

        if (!self) return;

        for (const auto& callback : self->m_mouseMoveCallbacks)
        {
            callback(Math::vec2(xpos, ypos));
        }
    }

    void GLFWInput::glfwWheelCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        std::cout << "Mouse wheel: (x " << xoffset << ", y " << yoffset << ")\n";
    }

    static std::shared_ptr<InputSystem> g_inputSystem{};

    std::shared_ptr<InputSystem> getInputSystem()
    {
        return g_inputSystem;
    }

    std::shared_ptr<InputSystem> InputSystem::create(const Render::Window* window)
    {
        if (!g_inputSystem) {
            g_inputSystem = std::make_shared<GLFWInput>(window);
        }

        return g_inputSystem;
    }
}
