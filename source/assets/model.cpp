#include "model.hpp"
#include "mesh.hpp"
#include "asset_manager.hpp"
#include "render/shader_program.hpp"

namespace DF::Assets
{
    void Model::draw()
    {
        for (Mesh& mesh : m_meshes)
        {
            Material material{ m_materials[mesh.getMaterialIndex()] };

            AssetManager::getTexture(material.diffuse)->bind(0);
            AssetManager::getTexture(material.specular)->bind(1);
            AssetManager::getShader(material.shader)->use();

            mesh.draw();
        }
    }
}
