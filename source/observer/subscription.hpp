#pragma once

#include <memory>

namespace DF
{
    template<typename T>
    class Observer;

    template<typename T>
    class Subscription
    {
        friend class Observer<T>;

        using Callback = std::function<void(T)>;

    public:
        Subscription(Callback callback) noexcept
            : m_callback{ callback }
        {
        }

        ~Subscription()
        {
            unsubscribe();
        }

        void unsubscribe()
        {
            m_callback = nullptr;
        }

        explicit operator bool() const { return !!m_callback; }

        void operator()(T value) { m_callback(value); }

        Callback getCallback() const { return m_callback; }

    private:
        Callback m_callback{};
    };
}
