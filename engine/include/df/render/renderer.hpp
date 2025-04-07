#pragma once

#include <memory>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "render/shader_program.hpp"
#include "entities/camera.hpp"

namespace DF::Render
{
    enum class DrawMode
    {
        FILL,
        LINE,
    };

    class Renderer
    {
    private:
        DrawMode m_drawMode{ DrawMode::FILL };
        std::unique_ptr<ShaderProgram> m_shaderProgram{};
        std::shared_ptr<Entity::Camera> m_camera{};

    public:
        Renderer() = delete;

        explicit Renderer(std::shared_ptr<Entity::Camera> camera);

        void render();

        void setDrawMode(DrawMode mode);

        DrawMode getDrawMode() const { return m_drawMode; }

        void setCamera(std::shared_ptr<Entity::Camera> camera) { m_camera = camera; }
    };
}
