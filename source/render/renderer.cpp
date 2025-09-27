#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <sail-c++/sail-c++.h>

#include "world/world.hpp"
#include "render/renderer.hpp"
#include "render/window.hpp"
#include "math/math.hpp"
#include "input/input.hpp"

#include "components/camera.hpp"
#include "components/transform.hpp"
#include "components/transform_matrix.hpp"
#include "components/model.hpp"
#include "components/point_light.hpp"

namespace DF::Render
{
    struct PointLightData
    {
        Math::vec4 position;

        Math::vec4 ambient;
        Math::vec4 diffuse;
        Math::vec4 specular;

        float constant;
        float linear;
        float quadratic;

        float padding;
    };

    Renderer::Renderer(World* world)
        : m_world{ world }
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

        Math::mat4 view{};

        /*
        * TODO: find a better place for systems like this
        * (probably need to register all systems inside world?)
        */
        m_world->forEach<Components::Camera, Components::Transform>(
            [this, &view](const auto& camera, const auto& transform)
            {
                if (camera.active && transform.getDirty())
                {
                    view = Math::lookAt(transform.getPosition(), transform.getForwardVector() + transform.getPosition(), Math::vec3(0.0, 0.1, 0.0));
                    m_shaderProgram->setUniform("uView", view);

                    const auto projection{ Math::perspective(Math::degToRad(camera.fov), m_size.width / m_size.height, camera.near, camera.far) };
                    m_shaderProgram->setUniform("uProjection", projection);
                }
            }
        );

        /*
        * This is a TransformMatrix component update system for caching transform matrix
        * calculations.
        */
        m_world->forEach<Components::Transform, Components::TransformMatrix>(
            [this](auto transform, auto& transformMatrix)
            {
                if (transform.getDirty())
                {
                    Math::mat4 modelMat{ Math::mat4(1.0) };
                    modelMat = Math::translateMat4(modelMat, transform.getPosition());

                    Math::vec3 rot{ transform.getRotation() };
                    modelMat = Math::rotateMat4(modelMat, rot.x, Math::vec3(1.0f, 0.0f, 0.0f));
                    modelMat = Math::rotateMat4(modelMat, rot.y, Math::vec3(0.0f, 1.0f, 0.0f));
                    modelMat = Math::rotateMat4(modelMat, rot.z, Math::vec3(0.0f, 0.0f, 1.0f));

                    modelMat = Math::scaleMat4(modelMat, transform.getScale());

                    transformMatrix.translation = modelMat;
                    transform.resetDirty();
                }
            }
        );

        /////////////////////////////  CUBE  /////////////////////////////

        m_shaderProgram->setUniform("uMaterial.diffuse", 0);
        m_shaderProgram->setUniform("uMaterial.specular", 1);
        m_shaderProgram->setUniform("uMaterial.shininess", 32.0f);

        m_world->forEach<Object, Components::Model, Components::TransformMatrix>(
            [this](auto obj, const auto& model, const auto& transform)
            {
                if (!obj.hasComponent<Components::PointLight>())
                {
                    m_shaderProgram->setUniform("uLightColor", Math::vec3(0.0f));
                    m_shaderProgram->setUniform("uModel", transform.translation);
                    model.mesh->draw();
                }

            }
        );

        m_shaderProgram->setUniform("uMaterial.diffuse", 3);
        m_shaderProgram->setUniform("uMaterial.specular", 3);
        m_shaderProgram->setUniform("uMaterial.emmisive", 3);

        m_world->forEach<Components::Model, Components::TransformMatrix, Components::PointLight>(
            [this](const auto& model, const auto& transform, const auto& light)
            {
                m_shaderProgram->setUniform("uLightColor", light.color);
                m_shaderProgram->setUniform("uModel", transform.translation);
                model.mesh->draw();
            }
        );

        /////////////////////////////  Lights SSBO  /////////////////////////////

        static GLuint pointLightsBuffer{};

        if (!pointLightsBuffer)
        {
            glCreateBuffers(1, &pointLightsBuffer);
        }

        std::vector<PointLightData> pointLights{};

        m_world->forEach<Components::PointLight, Components::Transform>(
            [this, &pointLights, &view](const auto& light, const auto& transform)
            {
                pointLights.emplace_back(
                    PointLightData
                    {
                        view * Math::vec4(transform.getPosition(), 1.0f),

                        Math::vec4(light.color * 0.05f, 1.0f),
                        Math::vec4(light.color * 1.5f, 1.0f),
                        Math::vec4(light.color, 1.0f),

                        light.constant,
                        light.linear,
                        light.quadratic,

                        1.0f,
                    }
                    );
            }
        );

        glNamedBufferData(
            pointLightsBuffer,
            sizeof(PointLightData) * pointLights.size(),
            pointLights.data(),
            GL_DYNAMIC_DRAW
        );

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, pointLightsBuffer);
        m_shaderProgram->setUniform("uPointLightsNum", static_cast<int>(pointLights.size()));

        /////////////////////////////////////////////////////////////////////////
    }

    void Renderer::setDrawMode(DrawMode mode)
    {
        m_drawMode = mode;
        glPolygonMode(GL_FRONT_AND_BACK, mode == DrawMode::FILL ? GL_FILL : GL_LINE);
    }

    void Renderer::setWindowSize(float width, float height)
    {
        m_size = { width, height };
    }
}
