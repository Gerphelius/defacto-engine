#pragma once

#include "utils/math.hpp"

namespace DF::Components
{
    /*
    * Internal component for caching entity basis vectors
    */

    struct TransformMatrix
    {
        Math::mat4 m_translation{ 1.0 };
    };
}
