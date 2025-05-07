#pragma once

#include <memory>

#include "render/shader_program.hpp"
#include "entities/camera.hpp"

#include "core/world.hpp"

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
        std::shared_ptr<Core::World> m_world{};

    public:
        explicit Renderer(std::shared_ptr<Entity::Camera> camera, std::shared_ptr<Core::World> world);

        void render();

        void setDrawMode(DrawMode mode);

        DrawMode getDrawMode() const { return m_drawMode; }

        void setCamera(std::shared_ptr<Entity::Camera> camera) { m_camera = camera; }
    };
}
