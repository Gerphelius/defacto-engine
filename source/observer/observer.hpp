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
        int m_lastValidIndex{ -1 };
    };
}
