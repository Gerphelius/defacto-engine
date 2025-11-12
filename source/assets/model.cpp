#include "model.hpp"
#include "asset_manager.hpp"
#include "render/shader_program.hpp"

#include <iostream>
#include <string>

namespace DF::Assets
{
    void Model::draw(const MaterialOverrides& overrides)
    {
        for (std::size_t i{}; i < m_meshes.size(); ++i)
        {
            Material material{ m_materials[m_meshes[i].getMaterialIndex()]};

            std::string diffuse{};
            std::string specular{};
            Shader shader{};

            const auto& matOverride{ overrides.find(i) };

            if (matOverride != overrides.end())
            {
                diffuse = matOverride->second.diffuse.value_or(*material.diffuse);
                specular = matOverride->second.specular.value_or(*material.specular);
                shader = matOverride->second.shader.value_or(*material.shader);
            }

            AssetManager::getTexture(diffuse)->bind(0);
            AssetManager::getTexture(specular)->bind(1);
            AssetManager::getShader(shader)->use();

            m_meshes[i].draw();
        }
    }
}
