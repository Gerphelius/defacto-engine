#pragma once

#include <imgui.h>

#include "components/point_light.hpp"

namespace DF::UI::Debug
{
    struct PointLight
    {
        static void render(Components::PointLight& component);
    };
}
