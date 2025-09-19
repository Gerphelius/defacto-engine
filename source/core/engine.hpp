#pragma once

#include <memory>
#include <chrono>

#include "render/renderer.hpp"
#include "render/window.hpp"
#include "input/input.hpp"

namespace DF
{
    class World;
}

namespace DF
{
    class Engine final
    {
    public:
        Engine() = delete;

        static void init();

        static void run();

        static float getDeltaTime();

        static World* getWorld();

        static Input* getInput();

    private:
        static bool s_initialized;
        static bool s_running;
        static float s_deltaTime;

        static std::unique_ptr<DF::Render::Window> s_window;
        static std::unique_ptr<Input> s_inputSystem;
        static std::unique_ptr<Render::Renderer> s_renderer;
        static std::unique_ptr<World> s_world;
    };
}
