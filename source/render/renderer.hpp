#pragma once

#include <memory>

namespace DF
{
    class World;
}

namespace DF::Render
{
    class Window;

    enum class DrawMode
    {
        FILL,
        LINE,
    };

    class Renderer
    {
    public:
        explicit Renderer(Window* window, World* world);

        void render();

        void setDrawMode(DrawMode mode);

        DrawMode getDrawMode() const { return m_drawMode; }

    private:
        DrawMode m_drawMode{ DrawMode::FILL };
        Window* m_window{};
        World* m_world{};
    };
}
