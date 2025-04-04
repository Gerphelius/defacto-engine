#include "entities/camera.h"

namespace DF::Entity
{
    void Camera::setAspectRatio(float ratio)
    {
        m_aspectRatio = ratio;
        m_projection = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near, m_far);
    }

    glm::vec3 Camera::move(glm::vec3 amount)
    {
        m_location += amount;
        m_translation = glm::translate(glm::mat4{ 1.0 }, m_location);

        return m_location;
    }

    void Camera::setLocation(glm::vec3 newLocation)
    {
        m_location = newLocation;
        m_translation = glm::translate(glm::mat4{ 1.0 }, m_location);
    }
}
