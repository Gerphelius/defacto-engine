#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <filesystem>

#include "model.hpp"
#include "texture.hpp"
#include "render/shader_program.hpp"

namespace DF::Assets
{
    enum class Shader
    {
        PHONG,
        UNLIT,
    };

    struct AssetHandle
    {
        std::string path{};
    };

    class AssetManager
    {
    public:
        static void init();

        static bool loadModel(const std::string& path);
        static void loadTexture(const std::string& path);

        static Model* getModel(const std::string& path);
        static Texture* getTexture(const std::string& path);

        static Render::ShaderProgram* getShader(Shader shader);

    private:
        static inline std::unordered_map<std::string, std::unique_ptr<Model>> s_models{};
        static inline std::unordered_map<std::string, std::unique_ptr<Texture>> s_textures{};
        static inline std::unordered_map<Shader, std::unique_ptr<Render::ShaderProgram>> s_shaders{};

        static inline std::filesystem::path m_assetsDirectory{ "../../resources/" };

        AssetManager() = delete;
    };
}
