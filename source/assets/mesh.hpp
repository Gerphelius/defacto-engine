#pragma once

#include <string>

#include "material.hpp"

namespace DF::Assets
{
    class Mesh
    {
    public:
        Mesh(const std::vector<float>& vertices, const std::vector<int>& indices, Material material);

        void draw() const;

    private:
        unsigned int m_vbo{};
        unsigned int m_ebo{};
        unsigned int m_vao{};
        int m_indicesCount{};
        Material m_material{};
    };
}
