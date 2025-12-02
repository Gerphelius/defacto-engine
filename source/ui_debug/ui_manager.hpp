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

    public:
        template <typename Widget>
        static inline void addWidget(const Widget& widget)
        {
            s_widgets.emplace_back(std::make_unique<Widget>(widget));
        }

    private:
        static inline std::vector<std::unique_ptr<IWidget>> s_widgets{};

        static void init(const Render::Window* window);

        static void render();

        static void destroy();
    };
}
