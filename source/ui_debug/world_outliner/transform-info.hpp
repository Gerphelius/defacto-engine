#pragma once

#include <imgui.h>

#include "components/transform.hpp"

namespace DF::UI::Debug
{
    struct TransformInfo
    {
        static void render(Components::TransformProxy& transform);
    };
}
