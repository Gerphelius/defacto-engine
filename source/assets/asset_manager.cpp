#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <sail-c++/sail-c++.h>

#include "asset_manager.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "material.hpp"
#include "texture.hpp"

// TODO: make a system to check reference count to assets and unload unused assets on next frame.

namespace DF::Assets
{
    std::unordered_map<std::string, std::unique_ptr<Model>> AssetManager::s_models{};
    std::unordered_map<std::string, std::unique_ptr<Texture>> AssetManager::s_textures{};
    std::vector<std::unique_ptr<Material>> AssetManager::s_materials{};

    std::filesystem::path AssetManager::m_assetsDirectory{ "../../resources/" };

    // TODO: need to create a clear initialization flow for all engine modules, because for now,
    //       if you try to create ShaderProgram as static when opengl/glad is not initialized,
    //       engine would crash.

    //Render::ShaderProgram AssetManager::m_phongShader
    //{
    //    "../../resources/shaders/phong.vert.glsl",
    //    "../../resources/shaders/phong.frag.glsl"
    //};

    void AssetManager::loadModel(const std::string& path)
    {
        auto it = s_models.find(path);

        if (it != s_models.end()) return;

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

            return;
        }

        std::vector<Mesh> meshes{};

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

            Material material{};

            if (aiScene->HasMaterials())
            {
                aiMaterial* aiMaterial{ aiScene->mMaterials[mesh->mMaterialIndex] };

                // TODO: Total garbage, remove asap

                aiString texPath{};
                aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);
                std::string realpath{ m_assetsDirectory.string() + "models/" + texPath.C_Str() };
                loadTexture(realpath);
                material.diffuseHandle = realpath;

                aiMaterial->GetTexture(aiTextureType_SPECULAR, 0, &texPath);
                realpath = m_assetsDirectory.string() + "models/" + texPath.C_Str();
                loadTexture(realpath);
                material.specularHandle = realpath;
            }

            meshes.emplace_back(Assets::Mesh{ vertices, indices, std::move(material) });
        }

        s_models[path] = std::make_unique<Assets::Model>(std::move(meshes));
    }

    void AssetManager::loadTexture(const std::string& path)
    {
        auto it = s_textures.find(path);

        if (it != s_textures.end()) return;

        sail::image image{ path };

        if (image.is_valid())
        {
            s_textures[path] = std::make_unique<Assets::Texture>(image.width(), image.height(), image.pixels());
        }
        else
        {
            constexpr unsigned char whitePixel[]{ 0xFF, 0xFF, 0xFF };

            s_textures[path] = std::make_unique<Assets::Texture>(1, 1, (void*)whitePixel);
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
            std::cout << "No cached texture found, loading " << path << '\n';

            loadTexture(path);
        }

        return s_textures[path].get();
    }
}
