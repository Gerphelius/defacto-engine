#pragma once

#include <functional>
#include <memory>
#include <map>
#include <vector>

#include "render/window.h"

namespace DF::Core {
    class Engine;
}

namespace DF::Input
{
    enum class Key
    {
        ESC,
        SPACE,
        ENTER,
        N,
        UNDEFINED,

        MAX_KEYS,
    };

    enum class MouseButton
    {
        LEFT,
        RIGHT,
        MIDDLE,
    };

    class InputSystem
    {
    public:
        virtual bool keyPressed(Key key) const = 0;

        virtual bool mouseButtonPressed(MouseButton button) const = 0;

        virtual void onKeyPress(Key key, std::function<void()> callback) = 0;

        virtual void onMouseButtonPress(MouseButton button, std::function<void()> callback) = 0;

        virtual ~InputSystem() = default;

    protected:
        static std::unique_ptr<InputSystem> create(const Render::Window* window);

        friend class Core::Engine;
    };

    //std::shared_ptr<InputSystem> getInputSystem();
}
