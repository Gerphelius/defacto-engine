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

        Projection m_projecton{ Projection::PERSPECTIVE };
        float m_fov{ 45.0f };
        bool m_active{};
        float m_near{ 0.1f };
        float m_far{ 10000.0f };
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
