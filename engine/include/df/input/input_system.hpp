#pragma once

#include <functional>
#include <memory>

#include "render/window.hpp"
#include "utils/math.hpp"

namespace DF::Core {
    class Engine;
}

namespace DF::Input
{
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
    };

    enum class KeyEvent
    {
        PRESS,
        RELEASE,
        HOLD,
    };

    class InputSystem
    {
    public:
        using KeyPressCallback = std::function<void()>;
        using MouseMoveCallback = std::function<void(Math::vec2)>;

        virtual bool keyPressed(Key key) const = 0;

        virtual bool mouseKeyPressed(MouseKey key) const = 0;

        virtual void onKeyPress(Key key, KeyEvent event, KeyPressCallback callback) = 0;

        virtual void onMouseKeyPress(MouseKey key, KeyPressCallback callback) = 0;

        virtual void onMouseMove(MouseMoveCallback callback) = 0;

        virtual void update() = 0;

        virtual ~InputSystem() = default;

    protected:
        static std::shared_ptr<InputSystem> create(const Render::Window* window);

        friend class Core::Engine;
    };

    std::shared_ptr<InputSystem> getInputSystem();
}
