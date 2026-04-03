#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <gl/glew.h>

#include "platform_window.hpp"

namespace DF::Platform
{

int g_glfwKeys[(int)(Key::MAX_KEYS)] { GLFW_KEY_UNKNOWN, GLFW_KEY_ESCAPE,     GLFW_KEY_SPACE,
                                       GLFW_KEY_ENTER,   GLFW_KEY_LEFT_SHIFT, GLFW_KEY_N,
                                       GLFW_KEY_W,       GLFW_KEY_S,          GLFW_KEY_A,
                                       GLFW_KEY_D,       GLFW_KEY_Q,          GLFW_KEY_E,
                                       GLFW_KEY_UP,      GLFW_KEY_DOWN,       GLFW_KEY_LEFT,
                                       GLFW_KEY_RIGHT };
KeyState g_keyStates[(int)(Key::MAX_KEYS)] {};

static bool KeyPressed(Key key)
{
    KeyState state = g_keyStates[(int)key];

    return state == KeyState::PRESSED;
}

static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    int keyDF = (int)Key::UNDEFINED;

    for (int i = 0; i < (int)(Key::MAX_KEYS); ++i)
    {
        if (g_glfwKeys[i] == key)
        {
            keyDF = i;

            break;
        }
    }

    switch (action)
    {
        case GLFW_PRESS:
        {
            g_keyStates[keyDF] = KeyState::PRESSED;
            break;
        }
        case GLFW_RELEASE:
        {
            g_keyStates[keyDF] = KeyState::RELEASED;
            break;
        }
        case GLFW_REPEAT:
        {
            g_keyStates[keyDF] = KeyState::HOLD;
            break;
        }
    }
}

static Math::Vec2 g_cursorPos;

static void GLFWCursorPosCallback(GLFWwindow* window, double x, double y)
{
    g_cursorPos = Math::Vec2 { (float)x, (float)y };
}

static Math::Vec2 GetCursorPos()
{
    return g_cursorPos;
}

KeyState g_mouseKeyStates[(int)(MouseKey::MAX_KEYS)] {};

static bool MouseKeyPressed(MouseKey key)
{
    KeyState state = g_mouseKeyStates[(int)key];

    return state == KeyState::PRESSED;
}

static void GLFWMouseKeyCallback(GLFWwindow* window, int key, int action, int mode)
{
    int keyDF = (int)Key::UNDEFINED;

    switch (key)
    {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            keyDF = (int)MouseKey::LEFT;
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            keyDF = (int)MouseKey::RIGHT;
            break;
        }
        case GLFW_MOUSE_BUTTON_MIDDLE:
        {
            keyDF = (int)MouseKey::MIDDLE;
            break;
        }
    }

    switch (action)
    {
        case GLFW_PRESS:
        {
            g_mouseKeyStates[keyDF] = KeyState::PRESSED;
            break;
        }
        case GLFW_RELEASE:
        {
            g_mouseKeyStates[keyDF] = KeyState::RELEASED;
            break;
        }
        case GLFW_REPEAT:
        {
            g_mouseKeyStates[keyDF] = KeyState::HOLD;
            break;
        }
    }
}

Math::Vec2 g_scrollPos { 0.0f, 0.0f };

static void GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    g_scrollPos.x += (float)xoffset;
    g_scrollPos.y += (float)yoffset;
}

Window CreateWindow(int width, int height, const char* title)
{
    GLFWwindow* glfwWindow {};

    if (!glfwInit())
    {
        std::cout << "Failed to initialize glfw window.\n";
    }

    // glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!glfwWindow)
    {
        std::cout << "Failed to create glfw window.\n";

        glfwTerminate();
    }

    glfwMakeContextCurrent(glfwWindow);
    //glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize glew. Call CreateWindow first.\n";
    }

    // glfwSetWindowUserPointer(window, ptr);

    glfwSetKeyCallback(glfwWindow, GLFWKeyCallback);
    glfwSetMouseButtonCallback(glfwWindow, GLFWMouseKeyCallback);
    glfwSetCursorPosCallback(glfwWindow, GLFWCursorPosCallback);
    glfwSetScrollCallback(glfwWindow, GLFWScrollCallback);

    for (int i = 0; i < (int)Key::MAX_KEYS; ++i)
    {
        g_keyStates[i] = KeyState::RELEASED;
    }

    for (int i = 0; i < (int)MouseKey::MAX_KEYS; ++i)
    {
        g_mouseKeyStates[i] = KeyState::RELEASED;
    }

    std::cout << "Window initialization complete.\n";

    return Window { glfwWindow };
}

bool WindowClosed(Window* window)
{
    return glfwWindowShouldClose((GLFWwindow*)window->handle);
}

void SwapBuffers(Window* window)
{
    glfwSwapBuffers((GLFWwindow*)window->handle);
    glfwPollEvents();
}

Size GetFramebufferSize(Window* window)
{
    int width, height;
    glfwGetFramebufferSize((GLFWwindow*)window->handle, &width, &height);

    return Size(width, height);
}

static double GetCurrentTime()
{
    return glfwGetTime();
}

}; // namespace DF::Platform
