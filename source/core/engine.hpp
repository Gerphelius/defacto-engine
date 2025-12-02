#pragma once

#include <memory>
#include <chrono>

#include "render/renderer.hpp"
#include "render/window.hpp"
#include "input/input.hpp"

namespace DF
{
    class World;

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
        static inline bool s_initialized{};
        static inline bool s_running{};
        static inline float s_deltaTime{};

        static inline std::unique_ptr<DF::Render::Window> s_window{};
        static inline std::unique_ptr<Input> s_inputSystem{};
        static inline std::unique_ptr<Render::Renderer> s_renderer{};
        static inline std::unique_ptr<World> s_world{};
    };
}
