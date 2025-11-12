#pragma once

#include <functional>
#include <optional>

#include <flecs.h>

//#include "components/metadata.hpp"

namespace DF
{
    /*
    * TODO:
    *   - Create id inside object to match flecs::entity id and use it instead  +
    *   - Remove forEachComponent and use raw flecs instead?
    */

    template <typename Component>
    inline Component& getProxy(Component& component)
    {
        return component;
    }

    template <typename Component>
    inline decltype(auto) findProxy(Component& component)
    {
        using DF::getProxy;

        return getProxy(component);
    }

    class World;

    class Object
    {
    public:
        using Id = std::uint64_t;

        template <typename Component>
        void addComponent(const Component& comp)
        {
            m_entity.set<Component>(comp);
        }

        template <typename Component>
        decltype(auto) getComponent()
        {
            Component* component{ m_entity.get_mut<Component>() };

            assert(component && "Object doesn't have this component.");

            if constexpr (requires(Component& t) { getProxy(t); })
            {
                return getProxy(*component);
            }
            else
            {
                return *component;
            }
        }

        template <typename Component>
        bool hasComponent()
        {
            return m_entity.has<Component>();
        }

        template <typename Func>
        void forEachComponent(Func&& callback)
        {
            m_entity.each(
                [&](flecs::id id)
                {
                    callback(id);
                }
            );
        }

        Id getId() const
        {
            return m_id;
        }

        explicit operator bool() const { return !!m_entity.is_valid(); }

    private:
        friend class World;

        Object(flecs::entity entity)
            : m_entity{ entity }
            , m_id{ entity.id() }
        {
            //addComponent(Components::Metadata{ entity.name().c_str() });
        }

        flecs::entity m_entity{};
        Id m_id{};
    };
}
