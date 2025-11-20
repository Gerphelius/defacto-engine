#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <utility>
#include <optional>

#include "mesh.hpp"
#include "material.hpp"

namespace DF::Assets
{
    class Model
    {
    public:
        explicit Model(std::vector<Mesh> meshes, std::vector<Material> materials) noexcept
            : m_meshes{ std::move(meshes) }
            , m_materials{ std::move(materials) }
        {
        }

        void draw(const MaterialOverrides& overrides);

        std::vector<Material>& getMaterials() { return m_materials; }

    private:
        std::vector<Mesh> m_meshes{};
        std::vector<Material> m_materials{};
    };
}
