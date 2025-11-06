#include <iostream>

#include "input_glfw.hpp"

namespace DF::Backend
{
    bool InputGLFW::keyPressed(Key key) const
    {
        return false;
    }

    bool InputGLFW::mouseKeyPressed(MouseKey key) const
    {
        return m_pressedMouseKeys.contains(key);
    }

    void InputGLFW::onKeyPress(Key key, KeyEvent event, KeyPressCallback callback)
    {
        m_keyCallbacksMap[key][event].push_back(std::move(callback));
    }

    void InputGLFW::onMouseKeyPress(MouseKey button, KeyPressCallback callback)
    {

    }

    void InputGLFW::onMouseMove(MouseMoveCallback callback)
    {
        m_mouseMoveCallbacks.push_back(callback);
    }

    void InputGLFW::update()
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

    Input::Key InputGLFW::getMappedKeyGLFW(int key) const
    {
        auto it{ m_glfwKeyMap.find(key) };

        if (it == m_glfwKeyMap.end()) return Key::UNDEFINED;

        return it->second;
    }

    void InputGLFW::keyCallback(GLFWKey key, GLFWKeyAction action)
    {
        if (action == GLFW_REPEAT) return;

        KeyEvent keyEvent{};
        auto dfKey{ getMappedKeyGLFW(key) };

        switch (action)
        {
        case GLFW_PRESS:
            keyEvent = KeyEvent::PRESS;
            m_pressedKeys.insert(dfKey);
            break;
        case GLFW_RELEASE:
            keyEvent = KeyEvent::RELEASE;
            m_pressedKeys.erase(dfKey);
            break;
        }

        auto callbacks{ m_keyCallbacksMap[dfKey][keyEvent] };

        for (const auto& callback : callbacks)
        {
            callback();
        }
    }

    void InputGLFW::mouseKeyCallback(GLFWKey key, GLFWKeyAction action)
    {
        MouseKey dfMouseKey{};

        switch (key)
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
            m_pressedMouseKeys.insert(dfMouseKey);
            break;
        case GLFW_RELEASE:
            m_pressedMouseKeys.erase(dfMouseKey);
            break;
        }
    }

    void InputGLFW::cursorCallback(double xpos, double ypos)
    {
        for (const auto& callback : m_mouseMoveCallbacks)
        {
            callback(Math::vec2(xpos, ypos));
        }
    }

    void InputGLFW::wheelCallback(double xoffset, double yoffset)
    {
        //std::cout << "Mouse wheel: (x " << xoffset << ", y " << yoffset << ")\n";
    }

    //void InputGLFW::glfwKeyCallback(GLFWwindow* window, int key, int, int action, int)
    //{
    //    auto* self = static_cast<InputGLFW*>(glfwGetWindowUserPointer(window));

    //    if (!self || action == GLFW_REPEAT) return;

    //    KeyEvent keyEvent{};
    //    auto dfKey{ self->getMappedKeyGLFW(key) };

    //    switch (action)
    //    {
    //    case GLFW_PRESS:
    //        keyEvent = KeyEvent::PRESS;
    //        self->m_pressedKeys.insert(dfKey);
    //        break;
    //    case GLFW_RELEASE:
    //        keyEvent = KeyEvent::RELEASE;
    //        self->m_pressedKeys.erase(dfKey);
    //        break;
    //    }

    //    auto callbacks{ self->m_keyCallbacksMap[dfKey][keyEvent] };

    //    for (const auto& callback : callbacks)
    //    {
    //        callback();
    //    }
    //}

    //void InputGLFW::glfwMouseCallback(GLFWwindow* window, int button, int action, int)
    //{
    //    auto* self = static_cast<InputGLFW*>(glfwGetWindowUserPointer(window));

    //    if (!self) return;

    //    MouseKey dfMouseKey{};

    //    switch (button)
    //    {
    //    case GLFW_MOUSE_BUTTON_LEFT:
    //        dfMouseKey = MouseKey::LEFT;
    //        break;
    //    case GLFW_MOUSE_BUTTON_RIGHT:
    //        dfMouseKey = MouseKey::RIGHT;
    //        break;
    //    case GLFW_MOUSE_BUTTON_MIDDLE:
    //        dfMouseKey = MouseKey::MIDDLE;
    //        break;
    //    }

    //    switch (action)
    //    {
    //    case GLFW_PRESS:
    //        self->m_pressedMouseKeys.insert(dfMouseKey);
    //        break;
    //    case GLFW_RELEASE:
    //        self->m_pressedMouseKeys.erase(dfMouseKey);
    //        break;
    //    }
    //}

    //void InputGLFW::glfwCursorCallback(GLFWwindow* window, double xpos, double ypos)
    //{
    //    auto* self = static_cast<InputGLFW*>(glfwGetWindowUserPointer(window));

    //    if (!self) return;

    //    for (const auto& callback : self->m_mouseMoveCallbacks)
    //    {
    //        callback(Math::vec2(xpos, ypos));
    //    }
    //}

    //void InputGLFW::glfwWheelCallback(GLFWwindow* window, double xoffset, double yoffset)
    //{
    //    std::cout << "Mouse wheel: (x " << xoffset << ", y " << yoffset << ")\n";
    //}
}
