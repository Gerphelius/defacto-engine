#include "model.hpp"
#include "mesh.hpp"

namespace DF::Assets
{
    void Model::draw()
    {
        for (Mesh& mesh : m_meshes)
        {
            mesh.draw();
        }
    }
}
