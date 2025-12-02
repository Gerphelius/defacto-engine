#include <limits.h>

#include <imgui.h>

#include "transform.hpp"
#include "math/math.hpp"

#include "fmt/format.h"

namespace DF::UI::Debug
{
    const std::string& Transform::getName() const
    {
        return m_name;
    }

    void Transform::render(void* component)
    {
        Components::TransformProxy transform{ *static_cast<Components::Transform*>(component) };

        if (ImGui::CollapsingHeader(m_name.c_str(), ImGuiTreeNodeFlags_None))
        {
            if (ImGui::BeginTable("##properties", 2, ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 2.0f);

                // TODO: Do not update transform if previous value doesn't change.

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Position");
                ImGui::TableNextColumn();
                int v_min = INT_MIN, v_max = INT_MAX;
                Math::vec3 position{ transform.getPosition() };
                ImGui::DragScalarN("##position", ImGuiDataType_Float, &position, 3, 0.01f, &v_min, &v_max);
                transform.setPosition(position);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Rotation");
                ImGui::TableNextColumn();
                Math::vec3 rotation{ transform.getRotation() };
                ImGui::DragScalarN("##rotation", ImGuiDataType_Float, &rotation, 3, 0.01f, &v_min, &v_max);
                transform.setRotation(rotation);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Scale");
                ImGui::TableNextColumn();
                Math::vec3 scale{ transform.getScale() };
                ImGui::DragScalarN("##scale", ImGuiDataType_Float, &scale, 3, 0.01f, &v_min, &v_max);
                transform.setScale(scale);

                ImGui::EndTable();
            }
        }
    }
}
