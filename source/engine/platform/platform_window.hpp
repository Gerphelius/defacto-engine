#pragma once

namespace DF::Platform
{
struct Window
{
    void* handle;
};

struct Size
{
    int width;
    int height;
};

enum class Key
{
    UNDEFINED,
    ESC,
    SPACE,
    ENTER,
    SHIFT_L,
    N,
    W,
    S,
    A,
    D,
    Q,
    E,
    UP,
    DOWN,
    LEFT,
    RIGHT,

    MAX_KEYS,
};

enum class MouseKey
{
    UNDEFINED,
    LEFT,
    RIGHT,
    MIDDLE,

    MAX_KEYS,
};

enum class KeyState
{
    UNKNOWN,
    PRESSED,
    RELEASED,
    HOLD,
};

Window CreateWindow(int width, int height, const char* title);
void SwapBuffers(Window* window);
bool WindowClosed(Window* window);
Size GetFramebufferSize(Window* window);
bool KeyPressed(Key key);
double GetCurrentTime();
Math::Vec2 GetCursorPos();
bool MouseKeyPressed(MouseKey key);

} // namespace DF::Platform