#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <sail-c++/sail-c++.h>

#include "asset_manager.hpp"
#include "mesh.hpp"
#include "default/cube.hpp"

// TODO: make a system to check reference count to assets and unload unused assets on next frame.

namespace DF::Assets
{
    void AssetManager::init()
    {
        constexpr unsigned char whitePixel[]{ 0xFF, 0xFF, 0xFF };
        s_textures["white"] = std::make_unique<Texture>(1, 1, TextureFormat::RGB, (void*)(whitePixel));

        s_shaders[ShaderType::PHONG] = std::make_unique<Shader>(
            "../../resources/shaders/phong.vert.glsl",
            "../../resources/shaders/phong.frag.glsl"
        );
        s_shaders[ShaderType::UNLIT] = std::make_unique<Shader>(
            "../../resources/shaders/unlit.vert.glsl",
            "../../resources/shaders/unlit.frag.glsl"
        );

        s_models["cube"] = std::make_unique<Assets::Default::Cube>();
    }

    bool AssetManager::loadModel(const std::string& path, const MaterialOverrides& materialOverrides)
    {
        auto it = s_models.find(path);

        if (it != s_models.end()) return true;

        Assimp::Importer importer{};
        const aiScene* aiScene = importer.ReadFile(
            path,
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType
        );

        if (!aiScene)
        {
            std::cout << "Assimp import failed: " << importer.GetErrorString() << '\n';

            return false;
        }

        std::vector<Mesh> meshes{};
        std::vector<Material> materials{};

        for (unsigned int meshIndex{}; meshIndex < aiScene->mNumMeshes; ++meshIndex)
        {
            std::vector<float> vertices{};
            std::vector<int> indices{};

            aiMesh* mesh{ aiScene->mMeshes[meshIndex] };

            for (unsigned int i{}; i < mesh->mNumVertices; ++i)
            {
                vertices.push_back(mesh->mVertices[i].x);
                vertices.push_back(mesh->mVertices[i].y);
                vertices.push_back(mesh->mVertices[i].z);

                if (mesh->HasNormals())
                {
                    vertices.push_back(mesh->mNormals[i].x);
                    vertices.push_back(mesh->mNormals[i].y);
                    vertices.push_back(mesh->mNormals[i].z);
                }

                if (mesh->HasTextureCoords(0))
                {
                    vertices.push_back(mesh->mTextureCoords[0][i].x);
                    vertices.push_back(mesh->mTextureCoords[0][i].y);
                }
            }

            for (unsigned int i{}; i < mesh->mNumFaces; ++i)
            {
                aiFace face = mesh->mFaces[i];

                for (unsigned int j = 0; j < face.mNumIndices; j++)
                {
                    indices.push_back(face.mIndices[j]);
                }
            }

            meshes.emplace_back(Assets::Mesh{ vertices, indices, mesh->mMaterialIndex });
        }

        if (aiScene->HasMaterials())
        {
            for (unsigned int i{}; i < aiScene->mNumMaterials; ++i)
            {
                aiMaterial* aiMaterial{ aiScene->mMaterials[i] };

                std::string name{ aiMaterial->GetName().C_Str() };
                std::string diffuse{};
                std::string specular{};

                if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
                {
                    aiString diffusePath{};
                    aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &diffusePath);
                    auto modelDir = std::filesystem::path(path).parent_path();
                    auto realpath{ modelDir / diffusePath.C_Str() };
                    loadTexture(realpath.string());
                    diffuse = realpath.string();
                }

                if (aiMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
                {
                    aiString specularPath{};
                    aiMaterial->GetTexture(aiTextureType_SPECULAR, 0, &specularPath);
                    auto modelDir = std::filesystem::path(path).parent_path();
                    auto realpath{ modelDir / specularPath.C_Str() };
                    loadTexture(realpath.string());
                    specular = realpath.string();
                }

                materials.emplace_back(Material{ name, diffuse, specular, ShaderType::PHONG });
            }
        }

        s_models[path] = std::make_unique<Assets::Model>(std::move(meshes), std::move(materials));

        return true;
    }

    void AssetManager::loadTexture(const std::string& path)
    {
        auto it = s_textures.find(path);

        if (it != s_textures.end()) return;

        sail::image image{ path };

        if (image.is_valid())
        {
            TextureFormat format{};

            switch (image.pixel_format())
            {
            case(SailPixelFormat::SAIL_PIXEL_FORMAT_BPP24_RGB):
                format = TextureFormat::RGB;
                break;
            case(SailPixelFormat::SAIL_PIXEL_FORMAT_BPP32_RGBA):
                format = TextureFormat::RGBA;
                break;
            }

            s_textures[path] = std::make_unique<Assets::Texture>(image.width(), image.height(), format, image.pixels());
        }
    }

    Model* AssetManager::getModel(const std::string& path)
    {
        // TODO: handle error state when model path is invalid.

        auto it = s_models.find(path);

        if (it == s_models.end())
        {
            std::cout << "No cached model found, loading " << path << '\n';

            loadModel(path);
        }

        return s_models[path].get();
    }

    Texture* AssetManager::getTexture(const std::string& path)
    {
        // TODO: handle error state when texture path is invalid.

        auto it = s_textures.find(path);

        if (it == s_textures.end())
        {
            //std::cerr << "No cached texture found, need to load it first: " << path << '\n';

            return s_textures["white"].get();
        }

        return s_textures[path].get();
    }

    Shader* AssetManager::getShader(ShaderType shader)
    {
        auto it = s_shaders.find(shader);

        return s_shaders[shader].get();
    }
}
