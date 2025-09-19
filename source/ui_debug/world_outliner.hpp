#pragma once

#include <vector>

#include <imgui.h>

#include "widget.hpp"

namespace DF
{
    class World;
}

namespace DF::UI::Debug
{
    class WorldOutliner : public IDebugWidget
    {
    private:
        std::string m_name{ "World Outliner" };
        World* m_world{};

    public:
        WorldOutliner() noexcept;

        const std::string& getName() const override;

        void render() override;
    };
}
