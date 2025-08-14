#pragma once

#include <vector>
#include <memory>

#include "widget.hpp"

namespace DF::Render
{
    class Window;
}

namespace DF::UI::Debug
{
    class UIManager
    {
        friend class Engine;

    private:
        static std::vector<std::unique_ptr<IDebugWidget>> s_widgets;

    private:
        static void init(const Render::Window* window);

        static void render();

        static void destroy();
    };
}
