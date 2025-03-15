#pragma once

#include <functional>
#include <memory>

#include "render/renderer.h"
#include "render/window.h"
#include "input/input_system.h"

namespace DF::Core
{
    class Engine
    {
    private:
        std::unique_ptr<Render::Window> m_window{};
        std::unique_ptr<Render::Renderer> m_renderer{};
        std::shared_ptr<Input::InputSystem> m_inputSystem{};
        bool m_running{ false };

    public:
        Engine();

        void run(std::function<void(double)> update);
    };
}

/*
    -- Give ability to create instances of input, window etc. but
       for input you need to inject window, for window renderer etc.;

    -- For renderer make class OpenGLRenderer and locator pattern
       for switching to different at runtime;
*/
