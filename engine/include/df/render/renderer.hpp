#pragma once

#include <memory>

#include "render/shader_program.hpp"
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
        Core::World* m_world{};

    public:
        explicit Renderer(Core::World* world);

        void render();

        void setDrawMode(DrawMode mode);

        DrawMode getDrawMode() const { return m_drawMode; }
    };
}
