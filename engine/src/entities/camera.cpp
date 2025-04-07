#include "entities/camera.hpp"

namespace DF::Entity
{
    void Camera::setAspectRatio(float ratio)
    {
        m_aspectRatio = ratio;
        m_projection = Math::perspective(Math::degToRad(m_fov), m_aspectRatio, m_near, m_far);
    }

    Math::vec3 Camera::move(Math::vec3 amount)
    {
        m_location += amount;
        m_translation = Math::translateMat4(Math::mat4{ 1.0 }, m_location);

        return m_location;
    }

    void Camera::setLocation(Math::vec3 newLocation)
    {
        m_location = newLocation;
        m_translation = Math::translateMat4(Math::mat4{ 1.0 }, m_location);
    }
}
