#pragma once

#include <string>
#include <unordered_map>
#include <array>
#include <memory>
#include <filesystem>

#include "model.hpp"
#include "texture.hpp"
#include "shader.hpp"

namespace DF::Assets
{
    struct AssetHandle
    {
        std::string path{};
    };

    class AssetManager
    {
    public:
        static void init();

        static bool loadModel(const std::string& path, const MaterialOverrides& materialOverrides = {});
        static void loadTexture(const std::string& path);

        static Model* getModel(const std::string& path);
        static Texture* getTexture(const std::string& path);

        static Shader* getShader(ShaderType shader);

        static const auto& getShaderNames() { return s_shaderToName; }

    private:
        static inline std::unordered_map<std::string, std::unique_ptr<Model>> s_models{};
        static inline std::unordered_map<std::string, std::unique_ptr<Texture>> s_textures{};
        static inline std::unordered_map<ShaderType, std::unique_ptr<Shader>> s_shaders{};
        static inline std::array<std::string, static_cast<int>(ShaderType::SHADERS_COUNT)> s_shaderToName{"Phong", "Unlit"};

        static inline std::filesystem::path m_assetsDirectory{ "resources/" };

        AssetManager() = delete;
    };
}
