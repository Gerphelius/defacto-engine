#include "entities/camera.hpp"

#include <algorithm>

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

    Math::vec3 Camera::rotate(Math::vec3 amount)
    {
        m_rotation -= amount;

        m_rotation.y = std::clamp(m_rotation.y, -89.0f, 89.0f);

        Math::vec3 direction{};
        direction.x = cos(Math::degToRad(m_rotation.x)) * cos(Math::degToRad(m_rotation.y));
        direction.y = sin(Math::degToRad(m_rotation.y));
        direction.z = sin(Math::degToRad(m_rotation.x)) * cos(Math::degToRad(m_rotation.y));

        m_forward = Math::normalize(direction);
        m_right = Math::normalize(glm::cross(m_forward, Math::vec3(0.0, 1.0, 0.0)));
        m_up = glm::cross(m_forward, m_right);

        return m_rotation;
    }

    void Camera::setLocation(Math::vec3 newLocation)
    {
        m_location = newLocation;
        m_translation = Math::translateMat4(Math::mat4{ 1.0 }, m_location);
    }

    void Camera::setRotation(Math::vec3 rotation)
    {
        rotate(m_rotation - rotation);
    }
}
