#pragma once

#include <string>

namespace DF::UI::Debug
{
    class IDebugWidget
    {
    public:
        virtual const std::string& getName() const = 0;

        virtual void render() = 0;

        virtual ~IDebugWidget() = default;
    };
}
