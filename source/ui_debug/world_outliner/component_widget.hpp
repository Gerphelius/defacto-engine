#pragma once

#include <string>

namespace DF::UI::Debug
{
    class IComponentWidget
    {
    public:
        virtual const std::string& getName() const = 0;

        virtual void render(void* component) = 0;

        virtual ~IComponentWidget() = default;
    };
}
