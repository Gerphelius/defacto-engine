#pragma once

#include <memory>

namespace DF
{
    class World;
}

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
        World* m_world{};

        /**
        * TODO: set window size on its initialization. Currently it is used to update
        *       camera aspect ration on window resize. Try to move this logic to camera
        *       and window instead.
        */
        WindowSize m_size{ 800, 600 };

    public:
        explicit Renderer(World* world);

        void render();

        void setDrawMode(DrawMode mode);

        void setWindowSize(float width, float height);

        DrawMode getDrawMode() const { return m_drawMode; }
    };
}
