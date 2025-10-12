#pragma once

#include <vector>
#include <string>
#include <utility>

namespace DF::Assets
{
    class Mesh;

    struct Material
    {
        std::string diffuse{};
        std::string specular{};
    };

    class Model
    {
    public:
        explicit Model(std::vector<Mesh> meshes, std::vector<Material> materials) noexcept
            : m_meshes{ std::move(meshes) }
            , m_materials{ std::move(materials)}
        {
        }

        void draw();

    private:
        std::vector<Mesh> m_meshes{};
        std::vector<Material> m_materials{};
    };
}
