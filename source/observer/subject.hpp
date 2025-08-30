#pragma once

#include "observer.hpp"

namespace DF
{
    template<typename T>
    class Subject : public Observer<T>
    {
    public:
        Subject() noexcept {}

        Observer<T>& asObserver()
        {
            return *this;
        }

        /*
        * if (loop index > last valid index)
        *    break loop
        * 
        * if (loop index <= last valid index && current sub is invalid)
        *    swap current sub with last valid    (swap is redundant if loop index = last valid index)
        *    decrement last valid index
        * 
        * evaluate sub
        * 
        */

        void emit(T value)
        {
            for (Subscription<T>& sub : this->m_subscriptions)
            {
                if (sub)
                {
                    sub(value);
                }
            }
        }
    };
}
