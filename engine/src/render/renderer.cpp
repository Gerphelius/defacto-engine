#include <iostream>

#include <sail-c++/sail-c++.h>

#include "render/renderer.hpp"
#include "render/window.hpp"
#include "utils/math.hpp"

namespace DF::Render {
    Renderer::Renderer(std::shared_ptr<Entity::Camera> camera)
        : m_camera{ camera }
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << '\n';
        }

        const float vertices[] = {
        // positions            |  normals           |  texture coords

/* 0 */    0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f,  // Front
/* 1 */    0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
/* 2 */   -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
/* 3 */   -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,

/* 4 */   -0.5f,  0.5f, -0.5f,    0.0f, 0.0f,-1.0f,   1.0f, 1.0f,  // Back
/* 5 */   -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,-1.0f,   1.0f, 0.0f,
/* 6 */    0.5f,  0.5f, -0.5f,    0.0f, 0.0f,-1.0f,   0.0f, 1.0f,
/* 7 */    0.5f, -0.5f, -0.5f,    0.0f, 0.0f,-1.0f,   0.0f, 0.0f,

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
        const int indices[] = {
        //  triangle 1  |  triangle 2
             0, 1, 2,       1, 3, 2,       // Front
             7, 6, 4,       7, 5, 4,       // Back

             8, 9, 10,      10, 9, 11,     // Right
             12, 13, 14,    14, 13, 15,    // Left

             16, 18, 19,    16, 17, 19,    // Top
             20, 22, 23,    20, 23, 21     // Bottom
        };

        GLuint vbo{};
        glGenBuffers(1, &vbo);

        GLuint ebo{};
        glGenBuffers(1, &ebo);

        GLuint vao{};
        glGenBuffers(1, &vao);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // Unbind current vao, vbo and ebo respectively
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        m_shaderProgram = std::make_unique<ShaderProgram>(
            "../../assets/shaders/base.vert.glsl",
            "../../assets/shaders/base.frag.glsl"
        );

        m_shaderProgram->use();

        /////////////////////////////  TEXTURES  /////////////////////////////

        sail::image container{ "../../assets/images/container.jpg" };
        sail::image face{ "../../assets/images/awesomeface.png" };
        face.mirror(SAIL_ORIENTATION_MIRRORED_VERTICALLY);

        if (container.is_valid() && face.is_valid())
        {
            GLuint textures[2]{};
            glGenTextures(2, textures);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[0]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, container.width(), container.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, container.pixels());
            glGenerateMipmap(GL_TEXTURE_2D);

            //glActiveTexture(GL_TEXTURE1);
            //glBindTexture(GL_TEXTURE_2D, textures[1]);

            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, face.width(), face.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, face.pixels());
            //glGenerateMipmap(GL_TEXTURE_2D);

            //m_shaderProgram->setUniform("uTexture2", 1);
        }

        //////////////////////////////////////////////////////////////////////

        glBindVertexArray(vao);
        glEnable(GL_DEPTH_TEST);
    }

    void Renderer::render()
    {
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time{ static_cast<float>(glfwGetTime()) };

        Math::vec3 lightColor{ Math::vec3(0.0f, 1.0f, 0.0f) };

        m_shaderProgram->setUniform("uView", m_camera->getTranslation());
        m_shaderProgram->setUniform("uProjection", m_camera->getProjection());

        /////////////////////////////  CUBE  /////////////////////////////

        Math::mat4 model{ Math::mat4(1.0) };
        model = Math::rotateMat4(model, time, Math::vec3(0.0, 1.0, 0.0));

        m_shaderProgram->setUniform("uModel", model);

        m_shaderProgram->setUniform("uLightPos", Math::vec3(1.0f));
        m_shaderProgram->setUniform("uLight.ambient", lightColor * 0.05f);
        m_shaderProgram->setUniform("uLight.diffuse", lightColor * 0.5f);
        m_shaderProgram->setUniform("uLight.specular", lightColor);

        m_shaderProgram->setUniform("uMaterial.ambient", Math::vec3(1.0f, 0.5f, 0.31f));
        m_shaderProgram->setUniform("uMaterial.diffuse", Math::vec3(1.0f, 0.5f, 0.31f));
        m_shaderProgram->setUniform("uMaterial.specular", Math::vec3(0.5f));
        m_shaderProgram->setUniform("uMaterial.shininess", 32.0f);

        glBindTexture(GL_TEXTURE_2D, 1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        /////////////////////////////  LIGHT  /////////////////////////////

        model = Math::mat4(1.0);
        //model = Math::rotateMat4(model, time, Math::vec3(0.0, 1.0, 0.0));
        model = Math::translateMat4(model, Math::vec3(1.0, 1.0, 1.0));
        model = Math::scaleMat4(model, Math::vec3(0.2, 0.2, 0.2));

        m_shaderProgram->setUniform("uLight.ambient", lightColor);
        m_shaderProgram->setUniform("uLight.diffuse", lightColor);

        m_shaderProgram->setUniform("uModel", model);
        m_shaderProgram->setUniform("uMaterial.ambient", Math::vec3(1.0f));
        m_shaderProgram->setUniform("uMaterial.diffuse", lightColor);
        m_shaderProgram->setUniform("uMaterial.specular", Math::vec3(1.0f));
        m_shaderProgram->setUniform("uMaterial.shininess", 32.0f);

        glBindTexture(GL_TEXTURE_2D, 0);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    void Renderer::setDrawMode(DrawMode mode)
    {
        m_drawMode = mode;
        glPolygonMode(GL_FRONT_AND_BACK, mode == DrawMode::FILL ? GL_FILL : GL_LINE);
    }
}
