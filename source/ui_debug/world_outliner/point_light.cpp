#include <string>

#include "point_light.hpp"

namespace DF::UI::Debug
{
    void PointLight::render(Components::PointLight& component)
    {
        if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_None))
        {
            if (ImGui::BeginTable("##properties", 2, ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 2.0f);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Color");
                ImGui::TableNextColumn();
                ImGui::ColorEdit3("##", (float*)&component.color, ImGuiColorEditFlags_None);

                ImGui::EndTable();
            }
        }
    }
}
