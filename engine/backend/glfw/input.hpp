#pragma once

#include <unordered_map>
#include <vector>
#include <unordered_set>

#include <GLFW/glfw3.h>

#include "../../include/df/input/input.hpp"

namespace DF::Backend
{
    class GLFWInput : public DF::Input
    {
    private:
        friend class WindowGLFW;

        using EventCallbacksMap = std::unordered_map<KeyEvent, std::vector<KeyPressCallback>>;
        using GLFWKey = int;
        using GLFWKeyAction = int;

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
        GLFWInput() noexcept = default;

        bool keyPressed(Key key) const override;

        bool mouseKeyPressed(MouseKey key) const override;

        void onKeyPress(Key key, KeyEvent event, KeyPressCallback) override;

        void onMouseKeyPress(MouseKey key, KeyPressCallback) override;

        void onMouseMove(MouseMoveCallback callback) override;

        void update() override;

    private:
        Key getMappedKeyGLFW(int key) const;

        void keyCallback(GLFWKey key, GLFWKeyAction action);

        void mouseKeyCallback(GLFWKey key, GLFWKeyAction action);

        void cursorCallback(double xpos, double ypos);

        void wheelCallback(double xoffset, double yoffset);
    };
}
