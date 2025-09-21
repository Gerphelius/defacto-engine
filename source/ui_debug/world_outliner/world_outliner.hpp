#pragma once

#include <vector>

#include <imgui.h>

#include "ui_debug/widget.hpp"
#include "world/object.hpp"

namespace DF
{
    class World;
}

namespace DF::UI::Debug
{
    class WorldOutliner : public IDebugWidget
    {

    public:
        WorldOutliner() noexcept;

        const std::string& getName() const override;

        void render() override;

    private:
        std::string m_name{ "World Outliner" };
        World* m_world{};
        Object::Id m_selectedObjectId{};
    };
}
