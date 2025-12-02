#pragma once

#include "component_widget.hpp"

namespace DF::UI::Debug
{
    struct Metadata : public IComponentWidget
    {
    public:
        const std::string& getName() const override;

        void render(void* component) override;

    private:
        std::string m_name{ "Metadata" };
    };
}
