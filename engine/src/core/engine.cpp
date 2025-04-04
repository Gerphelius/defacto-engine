#include "core/engine.h"
#include "entities/camera.h"

namespace DF::Core
{
    Engine::Engine()
    {
        auto debugCamera{ std::make_shared<Entity::Camera>() };
        debugCamera->setAspectRatio(800.0f / 600.0f);

        m_window = Render::Window::create(800, 600, "DeFacto");
        m_renderer = std::make_unique<Render::Renderer>(debugCamera);
        m_inputSystem = Input::InputSystem::create(m_window.get());

        m_inputSystem->onKeyPress(
            Input::Key::ESC,
            [this]() mutable {
                m_running = false;
            }
        );

        m_inputSystem->onKeyPress(
            Input::Key::N,
            [this]() {
                const auto currentMode{ m_renderer->getDrawMode() };
                const auto newMode{ currentMode == Render::DrawMode::fill ? Render::DrawMode::line : Render::DrawMode::fill };

                m_renderer->setDrawMode(newMode);
            }
        );

        m_inputSystem->onKeyPress(
            Input::Key::W,
            [debugCamera]() {
                debugCamera->move(glm::vec3(0.0, 0.0, 1.0));
            }
        );
        m_inputSystem->onKeyPress(
            Input::Key::S,
            [debugCamera]() {
                debugCamera->move(glm::vec3(0.0, 0.0, -1.0));
            }
        );
        m_inputSystem->onKeyPress(
            Input::Key::D,
            [debugCamera]() {
                debugCamera->move(glm::vec3(-1.0, 0.0, 0.0));
            }
        );
        m_inputSystem->onKeyPress(
            Input::Key::A,
            [debugCamera]() {
                debugCamera->move(glm::vec3(1.0, 0.0, 0.0));
            }
        );
    }

    void Engine::run(std::function<void(double)> update)
    {
        if (m_running) return;

        m_running = true;

        while (m_running && !m_window->closed())
        {
            m_renderer->render();
            m_window->update();

            update(123.45);
        }
    }
}
