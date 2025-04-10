﻿#include "core/engine.h"
#include "entities/camera.h"

namespace DF::Core
{
    Engine::Engine()
    {
        auto debugCamera{ std::make_shared<Entity::Camera>() };
        debugCamera->setAspectRatio(800.0f / 600.0f);
        auto cameraSpeed{ std::make_shared<float>(1.5f) };

        m_window = Render::Window::create(800, 600, "DeFacto");
        m_renderer = std::make_unique<Render::Renderer>(debugCamera);
        m_inputSystem = Input::InputSystem::create(m_window.get());

        m_inputSystem->onKeyPress(
            Input::Key::ESC,
            Input::KeyEvent::PRESS,
            [this]() mutable {
                m_running = false;
            }
        );

        m_inputSystem->onKeyPress(
            Input::Key::N,
            Input::KeyEvent::PRESS,
            [this]() {
                const auto currentMode{ m_renderer->getDrawMode() };
                const auto newMode{ currentMode == Render::DrawMode::FILL ? Render::DrawMode::LINE : Render::DrawMode::FILL };

                m_renderer->setDrawMode(newMode);
            }
        );

        /*
             ██████  █████  ███    ███ ███████ ██████   █████
            ██      ██   ██ ████  ████ ██      ██   ██ ██   ██
            ██      ███████ ██ ████ ██ █████   ██████  ███████
            ██      ██   ██ ██  ██  ██ ██      ██   ██ ██   ██
             ██████ ██   ██ ██      ██ ███████ ██   ██ ██   ██
        */
        m_inputSystem->onKeyPress(
            Input::Key::W,
            Input::KeyEvent::HOLD,
            [=, this]() {
                debugCamera->move(glm::vec3(0.0, 0.0, 1.0) * *cameraSpeed * m_deltaTime);
            }
        );
        m_inputSystem->onKeyPress(
            Input::Key::S,
            Input::KeyEvent::HOLD,
            [=, this]() {
                debugCamera->move(glm::vec3(0.0, 0.0, -1.0) * *cameraSpeed * m_deltaTime);
            }
        );
        m_inputSystem->onKeyPress(
            Input::Key::D,
            Input::KeyEvent::HOLD,
            [=, this]() {
                debugCamera->move(glm::vec3(-1.0, 0.0, 0.0) * *cameraSpeed * m_deltaTime);
            }
        );
        m_inputSystem->onKeyPress(
            Input::Key::A,
            Input::KeyEvent::HOLD,
            [=, this]() {
                debugCamera->move(glm::vec3(1.0, 0.0, 0.0) * *cameraSpeed * m_deltaTime);
            }
        );
        m_inputSystem->onKeyPress(
            Input::Key::SHIFT_L,
            Input::KeyEvent::PRESS,
            [=]() {
                *cameraSpeed = 10.0f;
            }
        );
        m_inputSystem->onKeyPress(
            Input::Key::SHIFT_L,
            Input::KeyEvent::RELEASE,
            [=]() {
                *cameraSpeed = 1.5f;
            }
        );
    }

    void Engine::run()
    {
        if (m_running) return;

        m_running = true;

        while (m_running && !m_window->closed())
        {
            const auto currentTime{ std::chrono::high_resolution_clock::now() };
            const std::chrono::duration<float> elapsed_seconds{ currentTime - m_prevTime };

            m_deltaTime = elapsed_seconds.count();
            m_prevTime = currentTime;

            //std::cout << "Delta time:" << m_deltaTime << '\n';

            m_renderer->render();
            m_window->update();
            m_inputSystem->update();
        }
    }
}
