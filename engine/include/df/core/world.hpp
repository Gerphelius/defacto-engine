#pragma once

#include <functional>
#include <iostream>
#include <cassert>
#include <optional>

#include <entt/entt.hpp>

#include "components/transform.hpp"
#include "components/transform_matrix.hpp"

namespace DF::Core
{
    template <typename Component>
    inline Component& getProxy(Component& component)
    {
        return component;
    }

    template <typename Component>
    inline decltype(auto) findProxy(Component& component)
    {
        using DF::Core::getProxy;

        return getProxy(component);
    }

    using Object = entt::entity;

    class World
    {
    private:
        entt::registry m_registry{};

    public:
        Object createObject()
        {
            return m_registry.create();
        }

        entt::registry& getRegistry() { return m_registry; }

        template <typename T>
        void addComponent(const Object& object, const T& component)
        {
            m_registry.emplace<T>(object, component);
        }

        template <typename T>
        void removeComponent(const Object& object)
        {
            m_registry.remove<T>(object);
        }

        template <typename T>
        auto getComponent(const Object& object)
        {
            T* component{ m_registry.try_get<T>(object) };

            if constexpr (requires(T & t) { getProxy(t); })
            {
                if (component)
                {
                    return std::optional{ getProxy(*component) };
                }

                return std::optional<std::decay_t<decltype(getProxy(*component))>>{};
            }
            else
            {
                return component;
            }
        }

        void spawnObject(const Object& object)
        {
            auto* transformComp{ m_registry.try_get<Components::Transform>(object) };

            assert(transformComp && "That object doesn't have transform component and cannot be spawned");

            if (transformComp)
            {
                Math::mat4 modelMat{ Math::mat4(1.0) };
                Components::TransformMatrix transformMatrix{};
                transformMatrix.m_translation = Math::translateMat4(modelMat, transformComp->m_position);

                addComponent<Components::TransformMatrix>(object, transformMatrix);
                addComponent<Components::TransformDirty>(object, Components::TransformDirty{});
            }
        }

        template <typename T>
        void updateComponent(const Object& object, std::function<void(T&)> func)
        {
            m_registry.patch<T>(object, func);
        }

        template <typename... Components, typename Callback>
        void forEach(Callback&& callback)
        {
            m_registry.view<Components...>().each(
                [this, callback](Components&... comps) {
                    callback(findProxy(comps)...);
                }
            );
        }

        //template <typename... Components, typename Callback>
        //void forEach(Callback&& callback)
        //{
        //    m_registry.view<Components...>().each(
        //        [this, callback](entt::entity entity, Components&... comps) {
        //            callback(entity, findProxy(comps)...);
        //        }
        //    );
        //}
    };
}
