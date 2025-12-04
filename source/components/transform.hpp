#pragma once

#include "math/math.hpp"

namespace DF::Components
{
    struct Transform
    {
        Math::vec3 position{ 0.0 };
        Math::vec3 rotation{ 0.0 };
        Math::vec3 scale{ 1.0 };

        bool m_dirty{ true };
    };

    /* 
    * Probably better to use this tag instead of dirty flag to reduce transform system update iterations.
    */
    struct TransformDirty
    {
        bool dirty{ true };   // Used to bypass flecs error that empty structs cannot be set, only add.
    };

    struct TransformProxy
    {
        Transform& m_transform;

        Math::vec3 getForwardVector() const
        {
            Math::vec3 direction{};
            direction.x = sin(Math::degToRad(m_transform.rotation.x)) * cos(Math::degToRad(m_transform.rotation.y));
            direction.y = sin(Math::degToRad(m_transform.rotation.y));
            direction.z = -cos(Math::degToRad(m_transform.rotation.x)) * cos(Math::degToRad(m_transform.rotation.y));

            return Math::normalize(direction);
        }

        Math::vec3 getRightVector() const
        {
            return Math::normalize(glm::cross(getForwardVector(), Math::vec3(0.0, 1.0, 0.0)));
        }

        Math::vec3 getUpVector() const
        {
            return glm::cross(getForwardVector(), getRightVector());
        }

        bool getDirty() const { return m_transform.m_dirty; }

        void resetDirty() { m_transform.m_dirty = false; }

        Math::vec3 getPosition() const { return m_transform.position; }

        void setPosition(const Math::vec3 newPosition)
        {
            m_transform.position = newPosition;
            m_transform.m_dirty = true;
        }

        void addPosition(const Math::vec3 position)
        {
            m_transform.position += position;
            m_transform.m_dirty = true;
        }

        Math::vec3 getRotation() const { return m_transform.rotation; }

        void setRotation(const Math::vec3 newRotation)
        {
            m_transform.rotation = newRotation;
            m_transform.m_dirty = true;
        }

        void addRotation(const Math::vec3 rotation)
        {
            m_transform.rotation += rotation;
            m_transform.m_dirty = true;
        }

        Math::vec3 getScale() const { return m_transform.scale; }

        void setScale(const Math::vec3 newScale)
        {
            m_transform.scale = newScale;
            m_transform.m_dirty = true;
        }

        void addScale(const Math::vec3 scale)
        {
            m_transform.scale += scale;
            m_transform.m_dirty = true;
        }
    };

    inline TransformProxy getProxy(Transform& component)
    {
        return TransformProxy{ component };
    }
}
