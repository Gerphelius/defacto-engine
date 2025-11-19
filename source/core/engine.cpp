#include <deque>
#include <cassert>
#include <iostream>

#include <spdlog/spdlog.h>
#include <imgui_impl_glfw.h>

#include "math/math.hpp"
#include "core/engine.hpp"
#include "world/world.hpp"
#include "backend/backend_factory.hpp"
#include "core/service_locator.hpp"
#include "ui_debug/ui_manager.hpp"
#include "assets/asset_manager.hpp"

#include "components/camera.hpp"
#include "components/transform.hpp"

namespace DF
{
    bool Engine::s_initialized{};
    bool Engine::s_running{};
    float Engine::s_deltaTime{};

    std::unique_ptr<Render::Window> Engine::s_window{};
    std::unique_ptr<Input> Engine::s_inputSystem{};
    std::unique_ptr<Render::Renderer> Engine::s_renderer{};
    std::unique_ptr<World> Engine::s_world{};

    void Engine::init()
    {
        if (s_initialized) return;

        s_window = Core::BackendFactory::createWindow(1280, 800, "DeFacto");
        s_inputSystem = Core::BackendFactory::createInput(s_window.get());

        Core::ServiceLocator::registerService(s_inputSystem.get());
        Input* input{ Core::ServiceLocator::getService<Input>() };

        s_world = std::make_unique<World>();
        s_renderer = std::make_unique<Render::Renderer>(s_world.get());
        UI::Debug::UIManager::init(s_window.get());
        Assets::AssetManager::init();

        s_window->setResizeCallback([](float width, float height) { s_renderer->setWindowSize(width, height); });

        input->onKeyPress(
            Input::Key::ESC,
            Input::KeyEvent::PRESS,
            []() mutable {
                s_running = false;
            }
        );

        input->onKeyPress(
            Input::Key::N,
            Input::KeyEvent::PRESS,
            []() {
                const auto currentMode{ s_renderer->getDrawMode() };
                const auto newMode{ currentMode == Render::DrawMode::FILL ? Render::DrawMode::LINE : Render::DrawMode::FILL };

                s_renderer->setDrawMode(newMode);
            }
        );

        s_initialized = true;
    }

    void Engine::run()
    {
        assert(s_initialized && "Engine need to be initialized first. Call Engine::init() before Engine::run().");

        if (s_running) return;

        s_running = true;

        auto engineStart{ std::chrono::high_resolution_clock::now() };
        std::chrono::high_resolution_clock::time_point prevTime{ engineStart };
        int totalFrames{};

        while (s_running && !s_window->closed())
        {
            const auto currentTime{ std::chrono::high_resolution_clock::now() };
            const std::chrono::duration<float> elapsed_seconds{ currentTime - prevTime };

            s_deltaTime = elapsed_seconds.count();
            prevTime = currentTime;

            s_renderer->render();
            UI::Debug::UIManager::render();
            s_window->update(s_deltaTime);
            s_inputSystem->update();

            ++totalFrames;
        }

        UI::Debug::UIManager::destroy();

        const std::chrono::duration<float> totalFrameTime{ std::chrono::high_resolution_clock::now() - engineStart };
        const float avgFps{ static_cast<float>(totalFrames) / totalFrameTime.count() };

        std::cout << "Avg FPS: " << avgFps << '\n';
    }

    float Engine::getDeltaTime()
    {
        return s_deltaTime;
    }

    World* Engine::getWorld()
    {
        return s_world.get();
    }

    Input* Engine::getInput()
    {
        return s_inputSystem.get();
    }
}
