#include <string>

#include <imgui.h>

#include "point_light.hpp"

#include "components/point_light.hpp"

namespace DF::UI::Debug
{
    const std::string& PointLight::getName() const
    {
        return m_name;
    }

    void PointLight::render(void* component)
    {
        auto* pointLight{ static_cast<Components::PointLight*>(component) };

        if (ImGui::CollapsingHeader(m_name.c_str(), ImGuiTreeNodeFlags_None))
        {
            if (ImGui::BeginTable("##properties", 2, ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 2.0f);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Color");
                ImGui::TableNextColumn();
                ImGui::ColorEdit3("##", (float*)&pointLight->color, ImGuiColorEditFlags_None);

                ImGui::EndTable();
            }
        }
    }
}
