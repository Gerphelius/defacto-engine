#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
        glm::mat4 m_projection{};
        glm::mat4 m_translation{ 1.0 };
        glm::vec3 m_location{ 0.0 };

        float m_fov{ 45.5f };
        float m_aspectRatio{ 16 / 9 };
        float m_near{ 0.1f };
        float m_far{ 100.0f };

    public:
        Camera() noexcept
        {
            m_projection = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near, m_far);
        }

        Camera(const Camera& camera) = delete;
        Camera operator=(const Camera& camera) = delete;

        glm::mat4 getTranslation() const { return m_translation; }

        glm::mat4 getProjection() const { return m_projection; }

        Projection getProjectionType() const { return m_projectionType; }

        glm::vec3 getLocation() const { return m_location; }

        void setAspectRatio(float ratio);

        glm::vec3 move(glm::vec3 amount);

        void setLocation(glm::vec3 location);
    };
}
