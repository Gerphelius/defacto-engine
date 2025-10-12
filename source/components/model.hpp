#pragma once

#include <string>
#include <vector>

#include "assets/mesh.hpp"

namespace DF::Components
{
    struct Model
    {
        std::string model{};
        std::vector<std::string> materials{};
    };
}
