#pragma once

#include <memory>
#include <chrono>

#include "render/renderer.hpp"
#include "render/window.hpp"
#include "input/input_system.hpp"
#include "entities/camera.hpp"
#include "world.hpp"

namespace DF::Core
{
    class Engine
    {
    private:
        std::unique_ptr<Render::Window> m_window{};
        std::unique_ptr<Render::Renderer> m_renderer{};
        std::shared_ptr<Input::InputSystem> m_inputSystem{};
        bool m_running{ false };
        float m_deltaTime{};
        std::chrono::high_resolution_clock::time_point m_prevTime{};
        std::shared_ptr<Entity::Camera> m_debugCamera{};  // TODO: Create camera component and get rid of this.
        std::shared_ptr<World> m_world{};

    public:
        Engine();

        void run();

        auto getDeltaTime() const { return m_deltaTime; }

        auto getWorld() const { return m_world; }
    };
}

/*
    -- Give ability to create instances of input, window etc. but
       for input you need to inject window, for window renderer etc.;

    -- For renderer make class OpenGLRenderer and locator pattern
       for switching to different at runtime;
*/
