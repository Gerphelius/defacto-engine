#include <vector>

#include "cube.hpp"
#include "assets/shader.hpp"

static const std::vector vertices{
    //        transforms           |  normals           |  texture coords

    /* 0 */    0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f,  // Front
    /* 1 */    0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
    /* 2 */   -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
    /* 3 */   -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,

    /* 4 */   -0.5f,  0.5f, -0.5f,    0.0f, 0.0f,-1.0f,    1.0f, 1.0f,  // Back
    /* 5 */   -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,-1.0f,    1.0f, 0.0f,
    /* 6 */    0.5f,  0.5f, -0.5f,    0.0f, 0.0f,-1.0f,    0.0f, 1.0f,
    /* 7 */    0.5f, -0.5f, -0.5f,    0.0f, 0.0f,-1.0f,    0.0f, 0.0f,

    /* 8 */    0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,  // Right
    /* 9 */    0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
    /* 10 */   0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
    /* 11 */   0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,

    /* 12 */  -0.5f,  0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,    1.0f, 1.0f,  // Left
    /* 13 */  -0.5f, -0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
    /* 14 */  -0.5f,  0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
    /* 15 */  -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,

    /* 16 */   0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,  // Top
    /* 17 */  -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
    /* 18 */   0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
    /* 19 */  -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,

    /* 20 */   0.5f, -0.5f,  0.5f,    0.0f,-1.0f, 0.0f,    1.0f, 1.0f,  // Bottom
    /* 21 */  -0.5f, -0.5f,  0.5f,    0.0f,-1.0f, 0.0f,    1.0f, 0.0f,
    /* 22 */   0.5f, -0.5f, -0.5f,    0.0f,-1.0f, 0.0f,    0.0f, 1.0f,
    /* 23 */  -0.5f, -0.5f, -0.5f,    0.0f,-1.0f, 0.0f,    0.0f, 0.0f,
};

static const std::vector indices{
    //   triangle 1  |  triangle 2
         0, 1, 2,       1, 3, 2,       // Front
         7, 6, 4,       7, 5, 4,       // Back

         8, 9, 10,      10, 9, 11,     // Right
         12, 13, 14,    14, 13, 15,    // Left

         16, 18, 19,    16, 17, 19,    // Top
         20, 22, 23,    20, 23, 21     // Bottom
};

namespace DF::Assets::Models
{
    Cube::Cube() noexcept
        : Assets::Model::Model(
            { Assets::Mesh{ vertices, indices, 0 } },
            { Assets::Material{ "default", "", "", Assets::ShaderType::PHONG }}
        )
    {
    }
}
