#include <iostream>
#include <array>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "render/renderer.hpp"
#include "render/window.hpp"
#include "world/world.hpp"
#include "math/math.hpp"
#include "input/input.hpp"
#include "assets/asset_manager.hpp"
#include "assets/model.hpp"

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
            "../../resources/shaders/phong.vert.glsl",
            "../../resources/shaders/phong.frag.glsl"
        );
        m_shaderProgram->use();

        glEnable(GL_DEPTH_TEST);
    }

    void Renderer::render()
    {
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time{ static_cast<float>(glfwGetTime()) };

        Math::mat4 view{}; // Used to transform point light position into view space for SSBO

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
                    Assets::Model* m{ Assets::AssetManager::getModel(model.model) };
                    m->draw();
                }
            }
        );

        m_world->forEach<Components::Model, Components::TransformMatrix, Components::PointLight>(
            [this](const auto& model, const auto& transform, const auto& light)
            {
                m_shaderProgram->setUniform("uLightColor", light.color);
                m_shaderProgram->setUniform("uModel", transform.translation);
                Assets::Model* m{ Assets::AssetManager::getModel(model.model) };
                m->draw();
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
                pointLights.emplace_back
                (
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
