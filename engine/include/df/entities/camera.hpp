#pragma once

#include "utils/math.hpp"

namespace DF::Entity
{
    enum class Projection
    {
        perspective,
        orthogonal,
    };

    class Camera
    {
    private:
        Projection m_projectionType{ Projection::perspective };
        Math::mat4 m_projection{};
        Math::mat4 m_translation{ 1.0 };
        Math::vec3 m_location{ 0.0 };
        Math::vec3 m_rotation{ 0.0 };

        Math::vec3 m_forward{ 0.0 };
        Math::vec3 m_right{ 0.0 };
        Math::vec3 m_up{ 0.0 };

        float m_fov{ 45.5f };
        float m_aspectRatio{ 16 / 9 };
        float m_near{ 0.1f };
        float m_far{ 10000.0f };

    public:
        Camera() noexcept
        {
            m_projection = Math::perspective(Math::degToRad(m_fov), m_aspectRatio, m_near, m_far);
        }

        Camera(const Camera& camera) = delete;
        Camera operator=(const Camera& camera) = delete;

        Math::mat4 getTranslation() const { return Math::lookAt(m_location, m_forward + m_location, Math::vec3(0.0, 0.1, 0.0)); }

        Math::mat4 getProjection() const { return m_projection; }

        Projection getProjectionType() const { return m_projectionType; }

        Math::vec3 getLocation() const { return m_location; }

        Math::vec3 getRotation() const { return m_rotation; }

        Math::vec3 getForwardVector() const { return m_forward; }

        Math::vec3 getRightVector() const { return m_right; }

        Math::vec3 getUpVector() const { return m_up; }

        void setAspectRatio(float ratio);

        Math::vec3 move(Math::vec3 amount);

        Math::vec3 rotate(Math::vec3 amount);

        void setLocation(Math::vec3 location);

        void setRotation(Math::vec3 rotation);
    };
}
