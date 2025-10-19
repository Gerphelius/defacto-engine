#include <string>

#include "model.hpp"
#include "assets/asset_manager.hpp"

namespace ImGui
{
    // ImGui::InputText() with std::string
    // Because text input needs dynamic resizing, we need to setup a callback to grow the capacity
    IMGUI_API bool  InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
}

namespace DF::UI::Debug
{
    void Model::render(Components::Model& component)
    {
        if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_None))
        {
            if (ImGui::BeginTable("##properties", 2, ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 2.0f);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Path");
                ImGui::TableNextColumn();

                static std::string path{};
                path = component.model;
                ImGui::InputText("##", &path, 128);
                ImGui::SameLine();

                if (ImGui::Button("Apply"))
                {
                    if (Assets::AssetManager::loadModel(path))
                    {
                        component.model = path;
                    }
                    else
                    {
                        path = component.model;
                    }
                }

                Assets::Model* model{ Assets::AssetManager::getModel(path) };
                const auto& materials{ model->getMaterials() };

                for (const auto& material : materials)
                {
                    ImGui::Text(material.name.c_str());
                }

                ImGui::EndTable();
            }
        }
    }
}
