#include <limits.h>

#include "transform-info.hpp"
#include "math/math.hpp"

namespace DF::UI::Debug
{
    void TransformInfo::render(Components::TransformProxy& transform)
    {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
        {
            if (ImGui::BeginTable("##properties", 2, ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 2.0f);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Position");
                ImGui::TableNextColumn();
                int v_min = INT_MIN, v_max = INT_MAX;
                Math::vec3 position{ transform.getPosition() };
                ImGui::DragScalarN("##position", ImGuiDataType_Float, &position, 3, 1.0f, &v_min, &v_max);
                transform.setPosition(position);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Rotation");
                ImGui::TableNextColumn();
                Math::vec3 rotation{ transform.getRotation() };
                ImGui::DragScalarN("##rotation", ImGuiDataType_Float, &rotation, 3, 1.0f, &v_min, &v_max);
                transform.setRotation(rotation);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Scale");
                ImGui::TableNextColumn();
                Math::vec3 scale{ transform.getScale() };
                ImGui::DragScalarN("##scale", ImGuiDataType_Float, &scale, 3, 1.0f, &v_min, &v_max);
                transform.setScale(scale);

                ImGui::EndTable();
            }
        }
    }
}
