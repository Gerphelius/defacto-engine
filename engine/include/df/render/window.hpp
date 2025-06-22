#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <functional>

namespace DF::Core{
    class Engine;
}

namespace DF::Render
{
    class Window
    {
    public:
        virtual void update(float deltaTime) = 0;

        virtual bool closed() const = 0;

        virtual void* getRawWindow() const = 0;

        virtual ~Window() = default;

    private:
        static std::unique_ptr<Window> create(int width, int height, std::string_view title);

        friend class Core::Engine;
    };
}
