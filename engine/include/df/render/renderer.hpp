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
        struct WindowSize
        {
            float width{};
            float height{};
        };

    private:
        DrawMode m_drawMode{ DrawMode::FILL };
        std::unique_ptr<ShaderProgram> m_shaderProgram{};
        Core::World* m_world{};
        WindowSize m_size{ 800, 600 };

    public:
        explicit Renderer(Core::World* world);

        void render();

        void setDrawMode(DrawMode mode);

        void setWindowSize(float width, float height);

        DrawMode getDrawMode() const { return m_drawMode; }
    };
}
