#pragma once

#include <imgui.h>

#include "components/model.hpp"

namespace DF::UI::Debug
{
    struct Model
    {
        static void render(Components::Model& component);
    };
}
