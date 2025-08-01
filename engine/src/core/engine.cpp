﻿#include <deque>
#include <numeric>
#include <cassert>

#include <spdlog/spdlog.h>
#include <imgui_impl_glfw.h>

#include "core/engine.hpp"
#include "core/backend_factory.hpp"
#include "utils/math.hpp"
#include "ui/debug.hpp"
#include "components/camera.hpp"
#include "components/transform.hpp"

namespace DF
{
    bool Engine::s_initialized{};
    bool Engine::s_running{};
    float Engine::s_deltaTime{};

    std::unique_ptr<DF::Render::Window> Engine::s_window{};
    std::shared_ptr<Input> Engine::s_inputSystem{};
    std::unique_ptr<Render::Renderer> Engine::s_renderer{};
    std::unique_ptr<Core::World> Engine::s_world{};

    void Engine::init()
    {
        if (s_initialized) return;

        s_window = Core::BackendFactory::createWindow(800, 600, "DeFacto");
        s_inputSystem = Core::BackendFactory::createInput(s_window.get());
        UI::Debug::init(s_window.get());

        s_world = std::make_unique<Core::World>();

        auto defaultCamera{ s_world->createObject() };

        s_world->addComponent<Components::Camera>(defaultCamera, Components::Camera{ .m_active{ true } });
        s_world->addComponent<Components::Transform>(defaultCamera, Components::Transform{});

        s_renderer = std::make_unique<Render::Renderer>(s_world.get());

        s_window->setResizeCallback([](float width, float height) { s_renderer->setWindowSize(width, height); });

        s_inputSystem->onKeyPress(
            Input::Key::ESC,
            Input::KeyEvent::PRESS,
            []() mutable {
                s_running = false;
            }
        );

        s_inputSystem->onKeyPress(
            Input::Key::N,
            Input::KeyEvent::PRESS,
            []() {
                const auto currentMode{ s_renderer->getDrawMode() };
                const auto newMode{ currentMode == Render::DrawMode::FILL ? Render::DrawMode::LINE : Render::DrawMode::FILL };

                s_renderer->setDrawMode(newMode);
            }
        );

        /*
             ██████  █████  ███    ███ ███████ ██████   █████
            ██      ██   ██ ████  ████ ██      ██   ██ ██   ██
            ██      ███████ ██ ████ ██ █████   ██████  ███████
            ██      ██   ██ ██  ██  ██ ██      ██   ██ ██   ██
             ██████ ██   ██ ██      ██ ███████ ██   ██ ██   ██
        */

        constexpr float rotationSpeed{ 0.3f };
        constexpr float maxCameraSpeed{ 40.0f };
        constexpr float normalCameraSpeed{ 10.0f };
        static float cameraSpeed{ normalCameraSpeed };

        s_inputSystem->onKeyPress(
            Input::Key::W,
            Input::KeyEvent::HOLD,
            [defaultCamera]() {
                auto transform{ s_world->getComponent<Components::Transform>(defaultCamera) };

                if (!transform) return;

                transform->addPosition(transform->getForwardVector() * cameraSpeed * s_deltaTime);
            }
        );
        s_inputSystem->onKeyPress(
            Input::Key::S,
            Input::KeyEvent::HOLD,
            [defaultCamera]() {
                auto transform{ s_world->getComponent<Components::Transform>(defaultCamera) };

                if (!transform) return;

                transform->addPosition(-transform->getForwardVector() * cameraSpeed * s_deltaTime);
            }
        );
        s_inputSystem->onKeyPress(
            Input::Key::D,
            Input::KeyEvent::HOLD,
            [defaultCamera]() {
                auto transform{ s_world->getComponent<Components::Transform>(defaultCamera) };

                if (!transform) return;

                transform->addPosition(transform->getRightVector() * cameraSpeed * s_deltaTime);
            }
        );
        s_inputSystem->onKeyPress(
            Input::Key::A,
            Input::KeyEvent::HOLD,
            [defaultCamera]() {
                auto transform{ s_world->getComponent<Components::Transform>(defaultCamera) };

                if (!transform) return;

                transform->addPosition(-transform->getRightVector() * cameraSpeed * s_deltaTime);
            }
        );

        s_inputSystem->onKeyPress(
            Input::Key::SHIFT_L,
            Input::KeyEvent::PRESS,
            [maxCameraSpeed]() mutable {
                cameraSpeed = maxCameraSpeed;
            }
        );
        s_inputSystem->onKeyPress(
            Input::Key::SHIFT_L,
            Input::KeyEvent::RELEASE,
            [normalCameraSpeed]() mutable {
                cameraSpeed = normalCameraSpeed;
            }
        );

        s_inputSystem->onMouseMove(
            [rotationSpeed, defaultCamera](Math::vec2 pos) {
                static Math::vec2 s_lastPos{};

                if (s_inputSystem->mouseKeyPressed(Input::MouseKey::RIGHT))
                {
                    Math::vec2 currentPos{ pos - s_lastPos };

                    auto transform{ s_world->getComponent<Components::Transform>(defaultCamera) };

                    if (!transform) return;

                    auto newRotation{ transform->getRotation() - Math::vec3(currentPos.x * -rotationSpeed, currentPos.y * rotationSpeed, 0.0)};
                    newRotation.y = std::clamp(newRotation.y, -89.0f, 89.0f);

                    transform->setRotation(newRotation);
                }

                s_lastPos = pos;
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
            UI::Debug::render();
            s_window->update(s_deltaTime);
            s_inputSystem->update();

            ++totalFrames;
        }

        const std::chrono::duration<float> totalFrameTime{ std::chrono::high_resolution_clock::now() - engineStart };
        const float avgFps{ static_cast<float>(totalFrames) / totalFrameTime.count() };

        std::cout << "Avg FPS: " << avgFps << '\n';
    }

    float Engine::getDeltaTime()
    {
        return s_deltaTime;
    }

    Core::World* Engine::getWorld()
    {
        return s_world.get();
    }
}
