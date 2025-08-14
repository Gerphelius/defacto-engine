#include "ui_debug/world_outliner.hpp"
#include "core/engine.hpp"
#include "core/world.hpp"
#include "components/metadata.hpp"

namespace DF::UI::Debug
{
    WorldOutliner::WorldOutliner() noexcept
    {
        m_world = Engine::getWorld();
    }

    const std::string& WorldOutliner::getName() const
    {
        return m_name;
    }

    void WorldOutliner::render()
    {
        ImGui::BeginChild(ImGui::GetID(1), ImVec2(ImGui::GetContentRegionAvail().x, 200.0f));

        std::vector<Core::Object> objects{};
        m_world->forEachObject<Components::Transform>(
            [&objects](Core::Object& obj, const auto&) {
                objects.push_back(obj);
            }
        );

        ImGuiListClipper clipper{};
        clipper.Begin(static_cast<int>(objects.size()));

        while (clipper.Step()) {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                auto obj = objects[i];
                auto metadata = m_world->getComponent<Components::Metadata>(obj);

                if (metadata) {
                    ImGui::Text(metadata->name.c_str());
                }
            }
        }

        ImGui::EndChild();
    }
}