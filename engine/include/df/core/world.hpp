#pragma once

#include <vector>
#include <array>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <iostream>

#include <entt/entt.hpp>

#include "utils/math.hpp"
#include "utils/sparse_set.hpp"
#include "assets/mesh.hpp"

// For entities recycling make an array of entities [A, A, A, A, D, D], where A - Alive, D - Dead.
// Also, need to store the index of last alive entity (LAE).
// When entity is deleted, swap last alive with that which will be deleted, mark is as D, shift LAE index.
// When entity is created, if LAE points to last array element, resize array and push new entity.
// If LAE not points to last array element, increment LAE and mark Dead entity as Alive.

namespace DF::Core
{
    using Object = entt::entity;

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

    class IComponentPool
    {
    public:
        virtual ~IComponentPool() = default;

        virtual std::size_t size() const = 0;

        virtual const std::vector<std::size_t> getObjects() const = 0;
    };

    template <typename T>
    class ComponentPool : public IComponentPool
    {
    private:
        Container::SparseSet<T> m_pool{};

    public:
        void addComponent(const Object& object, const T& component)
        {
            m_pool.add(object, component);
        }

        T& getComponent(const Object& object)
        {
            return m_pool.get(object);
        }

        const std::vector<std::size_t> getObjects() const override
        {
            return m_pool.getIndices();
        }

        std::size_t size() const override
        {
            return m_pool.size();
        }
    };

    class World
    {
    private:
        Container::SparseSet<Transform> m_transforms{};
        Container::SparseSet<Model> m_models{};
        std::vector<Object> m_objects{};

        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> m_componentPools{};


        /***********************************  EnTT  *****************************************/

        entt::registry m_registry{};


    public:
        Object createObject()
        {
            return m_registry.create();
        }

        template <typename T>
        void addComponent(const Object& object, const T& component)
        {
            m_registry.emplace<T>(object, component);
        }

        const entt::registry& getObjects() const
        {
            return m_registry;
        }

        /************************************************************************************/

    public:
        void spawnObject(const Object& object) const
        {
            // Render objects
        }

        //const std::vector<Object> getObjects() const
        //{
        //    return m_objects;
        //}

        //Object createObject()
        //{
        //    static Object lastObjectId{};

        //    ++lastObjectId;

        //    m_objects.push_back(lastObjectId);

        //    return lastObjectId;
        //}

        //template <typename T>
        //void addComponent(const Object& object, const T& component)
        //{
        //    auto& poolPtr = m_componentPools[std::type_index(typeid(T))];

        //    if (!poolPtr)
        //    {
        //        poolPtr = std::make_unique<ComponentPool<T>>();
        //    }

        //    static_cast<ComponentPool<T>*>(poolPtr.get())->addComponent(object, component);
        //}

        template <typename... Components>
        void addComponents(const Object& object, Components... args)
        {
            (addComponent(object, args), ...);
        }

        template <typename T>
        T* getComponent(const Object& object)
        {
            auto pool{ m_componentPools.find(std::type_index(typeid(T))) };

            if (pool != m_componentPools.end())
            {
                auto* typedPool{ static_cast<ComponentPool<T>*>(pool->second.get()) };

                return &typedPool->getComponent(object);
            }

            return nullptr;
        }

        template<typename... Components>
        auto getComponents(const Object& object) {
            return std::make_tuple(getComponent<Components>(object)...);
        }

        template<typename... Components, typename Callback>
        void forEach(const Callback& callback)
        {
            std::array<IComponentPool*, sizeof...(Components)> pools{
                m_componentPools[typeid(Components)].get()...
            };

            auto compare{ [](IComponentPool* a, IComponentPool* b) { return b->size() > a->size(); } };
            auto smallestPool{ std::ranges::min_element(pools, compare) };

            for (const auto& object : (*smallestPool)->getObjects())
            {
                Object obj{ static_cast<Object>(object) };

                if ((getComponent<Components>(obj) && ...))
                {
                    callback(*getComponent<Components>(obj)...);
                }
            }
        }
    };
}
