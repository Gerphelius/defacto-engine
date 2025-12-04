#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "subscription.hpp"

namespace DF
{
    template<typename T>
    class Observer
    {
    public:
        Observer() noexcept {}

        Subscription<T>& subscribe(Subscription<T>::Callback callback)
        {
            m_subscriptions.push_back({ callback });
            ++m_lastValidIndex;

            return m_subscriptions.back();
        }

    protected:
        std::vector<Subscription<T>> m_subscriptions{};
        /**
        * TODO: This is currently unused, need to refactor, so index can represent last valid subscription.
        *       If m_subscriptions contain dead subscriptions, they can be reused similar to pooling method.
        */
        int m_lastValidIndex{ -1 };
    };
}
