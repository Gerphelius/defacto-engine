#pragma once

#include <string>
#include <utility>
#include <functional>
#include <cassert>

#define FLECS_CUSTOM_BUILD // Don't build all addons
#define FLECS_CPP
#define FLECS_MODULE
#define FLECS_SYSTEM
#define FLECS_PIPELINE

#include <flecs.h>

#include "utils/function_traits.hpp"
#include "object.hpp"
#include "components/transform.hpp"
#include "components/transform_matrix.hpp"
#include "components/camera.hpp"

namespace DF
{
    class World
    {
    public:
        World() noexcept;

        Object createObject()
        {
            return Object{ m_world.entity() };
        }

        Object createObject(const std::string& name)
        {
            return Object{ m_world.entity(name.c_str()) };
        }

        Object getObject(Object::Id id)
        {
            return Object{ m_world.get_alive(id) };
        }

        template <typename T, typename... Components, typename Func>
        void forEach(Func&& callback)
        {
            // Previously component types has been deduced from callback arguments,
            // but because there are component proxies for some of them, findProxy() returns a ComponentProxy type
            // instead of plain Component type, so when forEach callback is specified like this:
            // 
            // forEach([](Camera& camera, Transform& transform) { ... })
            // 
            // and Transform component has proxy, compiler complains about types mismatch, because findProxy()
            // returns TransformProxy as a second argument, but if second argument is of type TransformProxy,
            // flecs can not find any entity with this component, because it was never added, because it is not
            // even a component.
            // 
            // Here type T is used as a workaround for deducing if forEach is used with or without Object
            // as first callback argument. The problem is that function_traits templates cannot deduce auto types
            // from callback, because types of components are listed inside forEach template itself for now, so
            // there is no reason to specify them twice, also inside callback arguments.
            // 
            // TODO: find a way to pass right component type to flecs even if it has proxy, by putting component
            //       type inside Proxy definition and pass it to flecs so it can find proper entities.

            ForEachImpl<T, Components...> forEachImpl{ m_world, callback };
        }

        void spawnObject(Object& object)
        {
            auto transform{ object.getComponent<Components::Transform>() };

            //assert(transformComp && "That object doesn't have transform component and cannot be spawned");

            Math::mat4 modelMat{ 1.0 };
            Components::TransformMatrix transformMatrix{};
            transformMatrix.translation = Math::translateMat4(modelMat, transform.getPosition());

            object.addComponent<Components::TransformMatrix>(transformMatrix);
            //object.addComponent<Components::TransformDirty>(Components::TransformDirty{});
        }

        template <typename Component>
        bool isComponentType(Object::Id id) const
        {
            return id == m_world.id<Component>();
        }

        flecs::world& getRaw() { return m_world; }

    private:
        flecs::world m_world{};

        //template <typename ArgsList>
        //struct ForEachImpl;

        template <typename FirstArg, typename... Rest>
        struct ForEachImpl
        {
            template <typename Func>
            ForEachImpl(flecs::world& world, Func&& callback)
            {
                if constexpr (std::is_same_v<std::remove_cvref_t<FirstArg>, DF::Object>)
                {
                    world.each(
                        [cb = std::forward<Func>(callback)](flecs::entity e, Rest&... rest)
                        {
                            cb(Object{ e }, findProxy(rest)...);
                        }
                    );
                }
                else
                {
                    world.each(
                        [cb = std::forward<Func>(callback)](FirstArg& first, Rest&... rest)
                        {
                            cb(findProxy(first), findProxy(rest)...);
                        }
                    );
                }
            }
        };

        // TODO: Refactor or find a better place for default camera
        void spawnDefaultCamera();
    };
}
