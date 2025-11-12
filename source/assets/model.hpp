#pragma once

#include <unordered_map>
#include <string>
#include <utility>
#include <optional>

#include "mesh.hpp"

namespace DF::Assets
{
    class Mesh;
    enum class Shader;

    struct Material
    {
        std::optional<std::string> name{};
        std::optional<std::string> diffuse{};
        std::optional<std::string> specular{};
        std::optional<Shader> shader{};
    };

    //struct Material
    //{
    //    std::string name{};
    //    std::string diffuse{};
    //    std::string specular{};
    //    Shader shader{};
    //};

    using MaterialOverrides = std::unordered_map<std::size_t, Material>;

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
