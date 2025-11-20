#pragma once

namespace DF::Assets
{
    class Mesh;
    enum class ShaderType;

    struct Material
    {
        std::optional<std::string> name{};
        std::optional<std::string> diffuse{};
        std::optional<std::string> specular{};
        std::optional<ShaderType> shader{};
    };

    //struct Material
    //{
    //    std::uint32_t name{};
    //    std::uint32_t diffuse{};
    //    std::uint32_t specular{};
    //    Shader shader{};
    //};

    using MaterialOverrides = std::unordered_map<std::size_t, Material>;
}
