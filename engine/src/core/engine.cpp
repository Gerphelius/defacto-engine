#include "core/engine.h"

namespace DF::Core
{
    Engine::Engine()
    {
        m_inputManager = Input::getInputManager();

        m_inputManager->onKeyPress(
            Input::Key::ESC,
            [this]() mutable {
                m_running = false;
            }
        );
    }

    void Engine::run(std::function<void(double)> update)
    {
        m_running = true;

        while (m_running)
        {
            m_renderer.render();

            update(123.45);
        }
    }
}
