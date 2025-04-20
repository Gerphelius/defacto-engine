#include <iostream>

#include <sail-c++/sail-c++.h>

#include "render/renderer.hpp"
#include "render/window.hpp"
#include "utils/math.hpp"
#include "input/input_system.hpp"

namespace DF::Render {
    Math::vec3 g_lightPos{ 1.0f };


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

        sail::image container{ "../../assets/images/container2.png" };
        sail::image container_spec{ "../../assets/images/container2_specular.png" };
        sail::image container_emissive{ "../../assets/images/matrix.jpg" };

        if (container.is_valid() && container_spec.is_valid())
        {
            GLuint textures[4]{};
            glGenTextures(4, textures);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[0]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, container.width(), container.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, container.pixels());
            glGenerateMipmap(GL_TEXTURE_2D);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, textures[1]);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, container_spec.width(), container_spec.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, container_spec.pixels());
            glGenerateMipmap(GL_TEXTURE_2D);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, textures[2]);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, container_emissive.width(), container_emissive.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, container_emissive.pixels());
            glGenerateMipmap(GL_TEXTURE_2D);

            constexpr unsigned char whitePixel[]{ 0xFF, 0xFF, 0xFF };
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, textures[3]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, whitePixel);
        }

        //////////////////////////////////////////////////////////////////////

        glBindVertexArray(vao);
        glEnable(GL_DEPTH_TEST);

        auto input{ Input::getInputSystem() };

        input->onKeyPress(
            Input::Key::UP,
            Input::KeyEvent::HOLD,
            [this]() mutable {
                g_lightPos.z += 0.1f;
            }
        );
        input->onKeyPress(
            Input::Key::DOWN,
            Input::KeyEvent::HOLD,
            [this]() mutable {
                g_lightPos.z -= 0.1f;
            }
        );
        input->onKeyPress(
            Input::Key::RIGHT,
            Input::KeyEvent::HOLD,
            [this]() mutable {
                g_lightPos.x += 0.1f;
            }
        );
        input->onKeyPress(
            Input::Key::LEFT,
            Input::KeyEvent::HOLD,
            [this]() mutable {
                g_lightPos.x -= 0.1f;
            }
        );
        input->onKeyPress(
            Input::Key::Q,
            Input::KeyEvent::HOLD,
            [this]() mutable {
                g_lightPos.y += 0.1f;
            }
        );
        input->onKeyPress(
            Input::Key::E,
            Input::KeyEvent::HOLD,
            [this]() mutable {
                g_lightPos.y -= 0.1f;
            }
        );
    }

    void Renderer::render()
    {
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time{ static_cast<float>(glfwGetTime()) };

        Math::vec3 lightColor{ 1.0f };
        Math::vec3 directionalLight{ 1.0f, -1.0f, 0.0f };

        m_shaderProgram->setUniform("uView", m_camera->getTranslation());
        m_shaderProgram->setUniform("uProjection", m_camera->getProjection());

        /////////////////////////////  CUBE  /////////////////////////////

        m_shaderProgram->setUniform("uTexture", 0);

        Math::mat4 model{ Math::mat4(1.0) };
        model = Math::rotateMat4(model, time, Math::vec3(0.0, 1.0, 0.0));

        m_shaderProgram->setUniform("uModel", model);

        // First way of doing flashlight type of spotlight
        //m_shaderProgram->setUniform("uLightDir", m_camera->getForwardVector());
        //m_shaderProgram->setUniform("uLightPos", m_camera->getLocation());

        // Second way of doing flashlight type of spotlight considering using view space in shader calculations
        m_shaderProgram->setUniform("uLightDir", Math::vec3(0.0f, 0.0f, -1.0f));
        m_shaderProgram->setUniform("uLightPos", Math::vec3(0.0f));

        m_shaderProgram->setUniform("uLight.ambient", lightColor * 0.05f);
        m_shaderProgram->setUniform("uLight.diffuse", lightColor * 1.5f);
        m_shaderProgram->setUniform("uLight.specular", lightColor);

        m_shaderProgram->setUniform("uLight.constant", 1.0f);
        m_shaderProgram->setUniform("uLight.linear", 0.14f);
        m_shaderProgram->setUniform("uLight.quadratic", 0.07f);
        m_shaderProgram->setUniform("uLight.innerCone", glm::cos(Math::degToRad(12.0f)));
        m_shaderProgram->setUniform("uLight.outerCone", glm::cos(Math::degToRad(15.0f)));

        m_shaderProgram->setUniform("uMaterial.diffuse", 0);
        m_shaderProgram->setUniform("uMaterial.specular", 1);
        m_shaderProgram->setUniform("uMaterial.emmisive", 2);
        m_shaderProgram->setUniform("uMaterial.shininess", 32.0f);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        /////////////////////////////  LIGHT  /////////////////////////////

        model = Math::mat4(1.0);
        //model = Math::rotateMat4(model, time, Math::vec3(0.0, 1.0, 0.0));
        model = Math::translateMat4(model, g_lightPos);
        model = Math::scaleMat4(model, Math::vec3(0.2, 0.2, 0.2));

        m_shaderProgram->setUniform("uLight.ambient", lightColor);
        m_shaderProgram->setUniform("uLight.diffuse", lightColor);

        m_shaderProgram->setUniform("uModel", model);
        m_shaderProgram->setUniform("uMaterial.diffuse", 3);
        m_shaderProgram->setUniform("uMaterial.specular", 3);
        m_shaderProgram->setUniform("uMaterial.shininess", 32.0f);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    void Renderer::setDrawMode(DrawMode mode)
    {
        m_drawMode = mode;
        glPolygonMode(GL_FRONT_AND_BACK, mode == DrawMode::FILL ? GL_FILL : GL_LINE);
    }
}
