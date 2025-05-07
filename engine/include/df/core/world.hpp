#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "utils/math.hpp"
#include "assets/mesh.hpp"

namespace DF::Core
{
    using Object = std::uint32_t;

    struct Transform
    {
        Math::vec3 m_position{ 0.0 };
        Math::vec3 m_rotation{ 1.0 };
        Math::vec3 m_scale{ 1.0 };
    };

    struct Model
    {
        std::shared_ptr<Assets::Mesh> m_mesh{};
    };

    class World
    {
    private:
        std::unordered_map<Object, Transform> m_transforms{};
        std::unordered_map<Object, Model>     m_models{};

    public:
        void spawnObject(const Object& object) const
        {
            // Render objects
        }

        const std::vector<Object> getObjects() const
        {

        }

        Object createObject() const
        {
            static Object lastObjectId{};

            return lastObjectId++;
        }

        void addTransform(const Object& entity, const Transform& transform)
        {
            m_transforms[entity] = transform;
        }

        void addModel(const Object& entity, const Model& model)
        {
            m_models[entity] = model;
        }

        Transform& getTransform(const Object& entity) { return m_transforms[entity]; }

        Model getModel(const Object& entity) { return m_models[entity]; }

        auto getModels() const { return m_models; }
    };
}
