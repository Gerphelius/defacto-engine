#pragma once

#include <functional>
#include <optional>

#include <flecs.h>

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
        using Id = std::uint64_t;

    public:
        template <typename Component>
        void addComponent(const Component& comp)
        {
            m_entity.set<Component>(comp);
        }

        template <typename Component>
        auto getComponent()
        {
            Component* component{ m_entity.get_mut<Component>() };

            assert(component && "Object doesn't have this component.");

            if constexpr (requires(Component& t) { getProxy(t); })
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

        void forEachComponent(std::function<void(Id)> callback)
        {
            m_entity.each(
                [&](flecs::id id)
                {
                    callback(id);
                }
            );
        }

    private:
        friend class World;

        Object(flecs::entity entity)
            : m_entity{ entity }
            , m_id{ entity.id() }
        {
        }

        flecs::entity m_entity{};
        Id m_id{};
    };
}
