#pragma once

#include <vector>

namespace DF::Assets
{
    class Mesh;

    class Model
    {
    public:
        explicit Model(std::vector<Mesh> meshes) noexcept
            : m_meshes{ std::move(meshes) }
        {
        }

        void draw();

    private:
        std::vector<Mesh> m_meshes{};
    };
}
