#pragma once

#include <functional>

#include "window.h"

namespace DF
{
    class Engine
    {
    public:
        Engine();

        void run(std::function<void(double)> update);

    private:
        Window m_window{ 800, 600, "DeFacto Engine" };
    };
}
