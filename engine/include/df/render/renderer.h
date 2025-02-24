#pragma once

#include <iostream>

#include "render/window.h"

namespace DF::Render
{
    class Renderer
    {
    public:
        explicit Renderer();

        void render();

    private:
        Window m_window{ 800, 600, "DeFacto Engine" };
    };
}
