#pragma once

#include <memory>

#include "assets/mesh.hpp"

namespace DF::Components
{
    struct Model
    {
        std::shared_ptr<Assets::Mesh> m_mesh{};
    };
}
