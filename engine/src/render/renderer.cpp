#include <iostream>
#include <cmath>

#include <sail-c++/sail-c++.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "render/renderer.h"
#include "render/window.h"

namespace DF::Render {
    Renderer::Renderer()
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << '\n';
        }

        const float vertices[] = {
            // positions         // colors           // texture coords
           -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
            0.5f,  0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
            0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
           -0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f
        };
        const int indices[] = {
            0, 1, 2,    // first triangle
            0, 2, 3     // second triangle
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

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, textures[1]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, face.width(), face.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, face.pixels());
            glGenerateMipmap(GL_TEXTURE_2D);

            m_shaderProgram->setUniform("uTexture2", 1);
        }

        //////////////////////////////////////////////////////////////////////

        glBindVertexArray(vao);
    }

    void Renderer::render()
    {
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float time{ static_cast<float>(glfwGetTime()) };

        glm::mat4 trans{ glm::mat4(1.0) };
        trans = glm::rotate(trans, time, glm::vec3(0.0, 0.0, 1.0));
        trans = glm::translate(trans, glm::vec3(0.5, 0.0, 0.0));
        trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.0));

        m_shaderProgram->setUniform("uTransform", trans);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        float scale{ sin(time * 10) * 0.25f + 0.5f };

        trans = glm::mat4(1.0);
        trans = glm::translate(trans, glm::vec3(-0.5, -0.5, 0.0));
        trans = glm::scale(trans, glm::vec3(scale, scale, 0.0));

        m_shaderProgram->setUniform("uTransform", trans);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    void Renderer::setDrawMode(DrawMode mode)
    {
        m_drawMode = mode;
        glPolygonMode(GL_FRONT_AND_BACK, mode == DrawMode::fill ? GL_FILL : GL_LINE);
    }
}
