#pragma once

#include <memory>
#include <chrono>

#include "render/renderer.hpp"
#include "render/window.hpp"
#include "input/input_system.hpp"
#include "world.hpp"

namespace DF
{
    class Engine final
    {
    public:
        Engine() = delete;

        static void init();

        static void run();

        static float getDeltaTime();

        static Core::World* getWorld();

    private:
        static bool s_initialized;
        static bool s_running;
        static float s_deltaTime;

        static std::unique_ptr<DF::Render::Window> s_window;
        static std::shared_ptr<Input::InputSystem> s_inputSystem;
        static std::unique_ptr<Render::Renderer> s_renderer;
        static std::unique_ptr<Core::World> s_world;
    };
}

/*
    -- Give ability to create instances of input, window etc. but
       for input you need to inject window, for window renderer etc.;

    -- For renderer make class OpenGLRenderer and locator pattern
       for switching to different at runtime;
*/
