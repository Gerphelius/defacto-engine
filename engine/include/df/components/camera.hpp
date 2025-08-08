#pragma once

namespace DF::Components
{
    struct Camera
    {
        enum class Projection
        {
            ORTHOGRAPHIC,
            PERSPECTIVE,
        };

        Projection projecton{ Projection::PERSPECTIVE };
        float fov{ 45.0f };
        bool active{};
        float near{ 0.1f };
        float far{ 10000.0f };
    };


    struct CameraProxy
    {
        Camera& m_camera;


    };

    //inline CameraProxy getProxy(Camera& component)
    //{
    //    return CameraProxy{ component };
    //}
}
