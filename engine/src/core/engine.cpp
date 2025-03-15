#include "core/engine.h"

namespace DF::Core
{
    Engine::Engine()
    {
        m_window = Render::Window::create(800, 600, "DeFacto");
        m_renderer = std::make_unique<Render::Renderer>();
        m_inputSystem = Input::InputSystem::create(m_window.get());

        m_inputSystem->onKeyPress(
            Input::Key::ESC,
            [this]() mutable {
                m_running = false;
            }
        );
    }

    void Engine::run(std::function<void(double)> update)
    {
        if (m_running) return;

        m_running = true;

        while (m_running && !m_window->closed())
        {
            m_window->update();
            m_renderer->render();

            update(123.45);
        }
    }
}
