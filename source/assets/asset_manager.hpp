#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <filesystem>

#include "render/shader_program.hpp"

namespace DF::Assets
{
    class Model;
    class Material;
    class Texture;

    struct AssetHandle
    {
        std::string path{};
    };

    class AssetManager
    {
    public:
        static void loadModel(const std::string& path);
        static void loadTexture(const std::string& path);

        static Model* getModel(const std::string& path);
        static Texture* getTexture(const std::string& path);

    private:
        static std::unordered_map<std::string, std::unique_ptr<Model>> s_models;
        static std::unordered_map<std::string, std::unique_ptr<Texture>> s_textures;
        static std::vector<std::unique_ptr<Material>> s_materials;
        //static Render::ShaderProgram m_phongShader;

        static std::filesystem::path m_assetsDirectory;

        AssetManager() = delete;
    };
}
