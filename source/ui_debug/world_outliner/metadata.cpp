#include <string>

#include <imgui.h>

#include "metadata.hpp"
#include "components/metadata.hpp"

namespace ImGui
{
    // ImGui::InputText() with std::string
    // Because text input needs dynamic resizing, we need to setup a callback to grow the capacity
    IMGUI_API bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
}

namespace DF::UI::Debug
{
    const std::string& Metadata::getName() const
    {
        return m_name;
    }

    void Metadata::render(void* component)
    {
        auto* metadata{ static_cast<Components::Metadata*>(component) };

        if (ImGui::CollapsingHeader(m_name.c_str(), ImGuiTreeNodeFlags_None))
        {
            if (ImGui::BeginTable("##properties", 2, ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 2.0f);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Name");
                ImGui::TableNextColumn();
                ImGui::InputText("##", &metadata->name, 128);

                ImGui::EndTable();
            }
        }
    }
}
