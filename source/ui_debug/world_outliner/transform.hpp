#pragma once

#include <string>

#include "component_widget.hpp"
#include "components/transform.hpp"

namespace DF::UI::Debug
{
    class Transform : public IComponentWidget
    {
    public:
        const std::string& getName() const override;

        void render(void* component) override;

    private:
        std::string m_name{ "Transform" };
    };
}
