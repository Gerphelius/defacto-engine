#pragma once

#include <functional>
#include <memory>
#include <map>
#include <vector>

#include "render/window.h"

namespace DF::Input
{
    enum class Key
    {
        ESC,
        SPACE,
        ENTER,
        UNDEFINED,

        MAX_KEYS,
    };

    enum class MouseButton
    {
        LEFT,
        RIGHT,
        MIDDLE,
    };

    class IInputManager
    {
    public:
        virtual bool keyPressed(Key key) const = 0;

        virtual bool mouseButtonPressed(Key key) const = 0;

        virtual void onKeyPress(Key key, std::function<void()> callback) = 0;

        virtual void onMouseButtonPress(MouseButton button, std::function<void()> callback) = 0;

        virtual void update() = 0;
    };

    std::shared_ptr<IInputManager> getInputManager();
}
