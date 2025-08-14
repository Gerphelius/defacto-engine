#pragma once

#include "math/math.hpp"

namespace DF::Components
{
    /*
    * Internal component for caching entity basis vectors
    */

    struct TransformMatrix
    {
        Math::mat4 translation{ 1.0 };
    };
}
