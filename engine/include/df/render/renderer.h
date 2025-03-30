#pragma once

#include <memory>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "render/shader_program.h"

namespace DF::Render
{
    enum class DrawMode
    {
        fill,
        line,
    };

    class Renderer
    {
    private:
        DrawMode m_drawMode{ DrawMode::fill };
        std::unique_ptr<ShaderProgram> m_shaderProgram{};

    public:
        explicit Renderer();

        void render();

        void setDrawMode(DrawMode mode);

        DrawMode getDrawMode() const { return m_drawMode; }
    };
}
