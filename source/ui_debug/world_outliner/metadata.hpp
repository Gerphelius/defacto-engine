#pragma once

#include <imgui.h>

#include "components/metadata.hpp"

namespace DF::UI::Debug
{
    struct Metadata
    {
        static void render(Components::Metadata& component);
    };
}
