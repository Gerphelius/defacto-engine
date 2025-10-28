#pragma once

#include <vector>
#include <string>
#include <utility>

namespace DF::Assets
{
    class Mesh;
    enum class Shader;

    struct Material
    {
        std::string name{};
        std::string diffuse{};
        std::string specular{};
        Shader shader{};
    };

    class Model
    {
    public:
        explicit Model(std::vector<Mesh> meshes, std::vector<Material> materials) noexcept
            : m_meshes{ std::move(meshes) }
            , m_materials{ std::move(materials) }
        {
        }

        void draw();

        std::vector<Material>& getMaterials() { return m_materials; }

    private:
        std::vector<Mesh> m_meshes{};
        std::vector<Material> m_materials{};
    };
}
