#include <string>

#include "model.hpp"
#include "assets/asset_manager.hpp"

namespace ImGui
{
    // ImGui::InputText() with std::string
    // Because text input needs dynamic resizing, we need to setup a callback to grow the capacity
    IMGUI_API bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
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
                auto& materials{ model->getMaterials() };

                for (auto& material : materials)
                {
                    const auto& shaders{ Assets::AssetManager::getShaderNames() };
                    const int currentShaderType{ static_cast<int>(material.shader) };
                    const char* selected = shaders[currentShaderType].c_str();
                    int selectedIndex = currentShaderType;

                    if (ImGui::BeginCombo("##xx", selected))
                    {
                        for (int i{}; i < shaders.size(); ++i)
                        {
                            const bool is_selected = (shaders[i] == selected);

                            if (ImGui::Selectable(shaders[i].c_str(), is_selected))
                            {
                                selected = shaders[i].c_str();
                                selectedIndex = i;
                            }

                            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                            if (is_selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }

                        ImGui::EndCombo();
                    }

                    if (selectedIndex != currentShaderType)
                    {
                        material.shader = static_cast<Assets::Shader>(selectedIndex);
                    }
                }

                ImGui::EndTable();
            }
        }
    }
}
