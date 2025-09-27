#pragma once

#include "math/math.hpp"

namespace DF::Components
{
    struct PointLight
    {
        Math::vec3 color{ 1.0f };

        float constant{ 1.0f };
        float linear{ 0.14f };
        float quadratic{ 0.07f };
    };
}
