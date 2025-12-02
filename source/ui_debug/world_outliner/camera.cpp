#include <string>

#include <imgui.h>

#include "camera.hpp"
#include "components/camera.hpp"

namespace DF::UI::Debug
{
    const std::string& Camera::getName() const
    {
        return m_name;
    }

    void Camera::render(void* component)
    {
        auto* metadata{ static_cast<Components::Camera*>(component) };

        if (ImGui::CollapsingHeader(m_name.c_str(), ImGuiTreeNodeFlags_None))
        {
            if (ImGui::BeginTable("##properties", 2, ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 2.0f);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("--");
                ImGui::TableNextColumn();
                ImGui::Text("No data");

                ImGui::EndTable();
            }
        }
    }
}
