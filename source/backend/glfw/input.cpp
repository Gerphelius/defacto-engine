#include <iostream>

#include "input.hpp"

namespace DF::Backend
{
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

    Input::Key GLFWInput::getMappedKeyGLFW(int key) const
    {
        auto it{ m_glfwKeyMap.find(key) };

        if (it == m_glfwKeyMap.end()) return Key::UNDEFINED;

        return it->second;
    }

    void GLFWInput::keyCallback(GLFWKey key, GLFWKeyAction action)
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

    void GLFWInput::mouseKeyCallback(GLFWKey key, GLFWKeyAction action)
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

    void GLFWInput::cursorCallback(double xpos, double ypos)
    {
        for (const auto& callback : m_mouseMoveCallbacks)
        {
            callback(Math::vec2(xpos, ypos));
        }
    }

    void GLFWInput::wheelCallback(double xoffset, double yoffset)
    {
        //std::cout << "Mouse wheel: (x " << xoffset << ", y " << yoffset << ")\n";
    }

    //void GLFWInput::glfwKeyCallback(GLFWwindow* window, int key, int, int action, int)
    //{
    //    auto* self = static_cast<GLFWInput*>(glfwGetWindowUserPointer(window));

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

    //void GLFWInput::glfwMouseCallback(GLFWwindow* window, int button, int action, int)
    //{
    //    auto* self = static_cast<GLFWInput*>(glfwGetWindowUserPointer(window));

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

    //void GLFWInput::glfwCursorCallback(GLFWwindow* window, double xpos, double ypos)
    //{
    //    auto* self = static_cast<GLFWInput*>(glfwGetWindowUserPointer(window));

    //    if (!self) return;

    //    for (const auto& callback : self->m_mouseMoveCallbacks)
    //    {
    //        callback(Math::vec2(xpos, ypos));
    //    }
    //}

    //void GLFWInput::glfwWheelCallback(GLFWwindow* window, double xoffset, double yoffset)
    //{
    //    std::cout << "Mouse wheel: (x " << xoffset << ", y " << yoffset << ")\n";
    //}
}
