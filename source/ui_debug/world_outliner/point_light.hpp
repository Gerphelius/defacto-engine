#pragma once

#include "component_widget.hpp"

namespace DF::UI::Debug
{
    class PointLight : public IComponentWidget
    {
    public:
        const std::string& getName() const override;

        void render(void* component) override;

    private:
        std::string m_name{ "Point Light" };
    };
}
