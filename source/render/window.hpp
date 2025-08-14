#pragma once

#include <functional>

namespace DF::Render
{
    class Window
    {
    public:
        using ResizeCallback = std::function<void(float, float)>;

        virtual void update(float deltaTime) = 0;

        virtual bool closed() const = 0;

        virtual void* getRawWindow() const = 0;

        virtual void setResizeCallback(ResizeCallback) = 0;

        virtual ~Window() = default;
    };
}
