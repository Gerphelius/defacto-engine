#pragma once

#include <string>

namespace DF::Assets
{
    class Mesh
    {
    public:
        Mesh(const std::vector<float>& vertices, const std::vector<int>& indices, unsigned int materialIndex);

        void draw() const;

        int getMaterialIndex() const { return m_materialIndex; }

    private:
        unsigned int m_vbo{};
        unsigned int m_ebo{};
        unsigned int m_vao{};
        int m_indicesCount{};
        unsigned int m_materialIndex{};
    };
}
