#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <sail-c++/sail-c++.h>

#include "render/renderer.hpp"
#include "render/window.hpp"
#include "utils/math.hpp"
#include "input/input_system.hpp"

namespace DF::Render {
    Math::vec3 g_lightPos{ 1.0f };


    Renderer::Renderer(std::shared_ptr<Entity::Camera> camera, std::shared_ptr<Core::World> world)
        : m_camera{ camera }
        , m_world{ world }
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << '\n';
        }

        m_shaderProgram = std::make_unique<ShaderProgram>(
            "../../resources/shaders/base.vert.glsl",
            "../../resources/shaders/base.frag.glsl"
        );

        m_shaderProgram->use();

        /////////////////////////////  TEXTURES  /////////////////////////////

        sail::image container{ "../../resources/images/container2.png" };
        sail::image container_spec{ "../../resources/images/container2_specular.png" };
        sail::image container_emissive{ "../../resources/images/matrix.jpg" };

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

        glEnable(GL_DEPTH_TEST);
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

        const auto& objects{ m_world->getObjects() };

        auto view{ objects.view<const Core::Model, Core::Transform>() };

        view.each(
            [this](const auto& model, const auto& transform) {
                Math::mat4 modelMat{ Math::mat4(1.0) };

                modelMat = Math::translateMat4(modelMat, transform.m_position);

                m_shaderProgram->setUniform("uModel", modelMat);

                model.m_mesh->draw();
            }
        );

        //auto objects{ m_world->getObjects() };

        //for (const auto& object : objects)
        //{
        //    Math::mat4 modelMat{ Math::mat4(1.0) };

        //    modelMat = Math::translateMat4(modelMat, m_world->getComponent<Core::Transform>(object)->m_position);

        //    m_shaderProgram->setUniform("uModel", modelMat);

        //    m_world->getComponent<Core::Model>(object)->m_mesh->draw();
        //}

        //m_world->forEach<Core::Model, Core::Transform>(
        //    [this](Core::Model& model, Core::Transform& transform) {
        //        Math::mat4 modelMat{ Math::mat4(1.0) };

        //        modelMat = Math::translateMat4(modelMat, transform.m_position);

        //        m_shaderProgram->setUniform("uModel", modelMat);

        //        model.m_mesh->draw();
        //    }
        //);

        m_shaderProgram->setUniform("uTexture", 0);
        m_shaderProgram->setUniform("uMaterial.diffuse", 3);
        m_shaderProgram->setUniform("uMaterial.specular", 3);
        m_shaderProgram->setUniform("uMaterial.shininess", 32.0f);
        m_shaderProgram->setUniform("uLight.ambient", lightColor);
        m_shaderProgram->setUniform("uLight.diffuse", lightColor);



        //// First way of doing flashlight type of spotlight
        ////m_shaderProgram->setUniform("uLightDir", m_camera->getForwardVector());
        ////m_shaderProgram->setUniform("uLightPos", m_camera->getLocation());

        //// Second way of doing flashlight type of spotlight considering using view space in shader calculations
        //m_shaderProgram->setUniform("uLightDir", Math::vec3(0.0f, 0.0f, -1.0f));
        //m_shaderProgram->setUniform("uLightPos", Math::vec3(0.0f));

        //m_shaderProgram->setUniform("uLight.ambient", lightColor * 0.05f);
        //m_shaderProgram->setUniform("uLight.diffuse", lightColor * 1.5f);
        //m_shaderProgram->setUniform("uLight.specular", lightColor);

        //m_shaderProgram->setUniform("uLight.constant", 1.0f);
        //m_shaderProgram->setUniform("uLight.linear", 0.14f);
        //m_shaderProgram->setUniform("uLight.quadratic", 0.07f);
        //m_shaderProgram->setUniform("uLight.innerCone", glm::cos(Math::degToRad(12.0f)));
        //m_shaderProgram->setUniform("uLight.outerCone", glm::cos(Math::degToRad(15.0f)));

        //m_shaderProgram->setUniform("uMaterial.diffuse", 0);
        //m_shaderProgram->setUniform("uMaterial.specular", 1);
        //m_shaderProgram->setUniform("uMaterial.emmisive", 2);
        //m_shaderProgram->setUniform("uMaterial.shininess", 32.0f);

        ///////////////////////////////  LIGHT  /////////////////////////////

        //model = Math::mat4(1.0);
        ////model = Math::rotateMat4(model, time, Math::vec3(0.0, 1.0, 0.0));
        //model = Math::translateMat4(model, g_lightPos);
        //model = Math::scaleMat4(model, Math::vec3(0.2, 0.2, 0.2));

        //m_shaderProgram->setUniform("uLight.ambient", lightColor);
        //m_shaderProgram->setUniform("uLight.diffuse", lightColor);

        //m_shaderProgram->setUniform("uModel", model);
        //m_shaderProgram->setUniform("uMaterial.diffuse", 3);
        //m_shaderProgram->setUniform("uMaterial.specular", 3);
        //m_shaderProgram->setUniform("uMaterial.shininess", 32.0f);

        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    void Renderer::setDrawMode(DrawMode mode)
    {
        m_drawMode = mode;
        glPolygonMode(GL_FRONT_AND_BACK, mode == DrawMode::FILL ? GL_FILL : GL_LINE);
    }
}
