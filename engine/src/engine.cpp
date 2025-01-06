#include "engine.h"

DF::Engine::Engine()
{
    m_window.init();
}

void DF::Engine::run(std::function<void(double)> update)
{
    while (m_window && !m_window.shouldClose())
    {
        m_window.update();

        update(123.45);
    }
}
