#pragma once

#include <any>
#include <unordered_map>
#include <typeindex>

namespace DF::Core
{
    class ServiceLocator
    {
    private:
        static inline std::unordered_map<std::type_index, std::any> s_services{};

    public:
        template<typename Service>
        static void registerService(Service* service)
        {
            s_services[std::type_index(typeid(Service))] = service;
        }

        template<typename Service>
        static Service* getService()
        {
            auto it{ s_services.find(std::type_index(typeid(Service))) };

            if (it != s_services.end())
            {
                return std::any_cast<Service*>(it->second);
            }

            return nullptr;
        }
    };
}
