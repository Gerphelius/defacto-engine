#pragma once

#include <string>
#include <vector>

#include "assets/model.hpp"

namespace DF::Components
{
    struct Model
    {
        std::string model{};
        Assets::MaterialOverrides materialOverrides{};
    };
}
