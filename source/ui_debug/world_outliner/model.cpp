#include <string>

#include "fmt/format.h"

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
            std::string path{ component.model };

            if (ImGui::BeginTable("##properties", 2, ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 2.0f);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Path");
                ImGui::TableNextColumn();

                static std::string newPath{ path };
                ImGui::InputText("##", &newPath, 128);
                ImGui::SameLine();

                if (ImGui::Button("Apply"))
                {
                    if (Assets::AssetManager::loadModel(path))
                    {
                        component.model = newPath;
                    }
                    else
                    {
                        newPath = component.model;
                    }
                }

                ImGui::EndTable();
            }

            ImGui::SeparatorText("Materials");

            Assets::Model* model{ Assets::AssetManager::getModel(path) };
            auto& materials{ model->getMaterials() };

            for (int i{}; i < materials.size(); ++i)
            {
                ImGui::TextUnformatted(fmt::format("{}: {}", i, *materials[i].name).c_str());

                std::string diffuse{};
                std::string specular{};
                Assets::Shader shader{};

                const auto& matOverride{ component.materialOverrides.find(i) };

                if (matOverride != component.materialOverrides.end())
                {
                    diffuse = matOverride->second.diffuse.value_or(*materials[i].diffuse);
                    specular = matOverride->second.specular.value_or(*materials[i].specular);
                    shader = matOverride->second.shader.value_or(*materials[i].shader);
                }

                if (ImGui::BeginTable("##properties", 2, ImGuiTableFlags_Resizable))
                {
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 2.0f);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("Diffuse");
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(diffuse.data());

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("Specular");
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(specular.data());

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("Shader");
                    ImGui::TableNextColumn();

                    const auto& shaders{ Assets::AssetManager::getShaderNames() };
                    const int currentShaderType{ static_cast<int>(shader) };
                    ImGui::TextUnformatted(shaders[currentShaderType].c_str());

                    //for (int i{}; i < materials.size(); ++i)
                    //{
                    //    const auto& shaders{ Assets::AssetManager::getShaderNames() };
                    //    const int currentShaderType{ static_cast<int>(*materials[i].shader) };
                    //    const char* selected = shaders[currentShaderType].c_str();
                    //    int selectedIndex = currentShaderType;

                    //    ImGui::PushID(i);
                    //    ImGui::TextUnformatted(materials[i].name.value().c_str());
                    //    ImGui::SameLine();

                    //    if (ImGui::BeginCombo("##shader", selected))
                    //    {
                    //        for (int j{}; j < shaders.size(); ++j)
                    //        {
                    //            const bool is_selected = (shaders[j] == selected);

                    //            if (ImGui::Selectable(shaders[j].c_str(), is_selected))
                    //            {
                    //                selected = shaders[j].c_str();
                    //                selectedIndex = j;
                    //            }

                    //            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    //            if (is_selected)
                    //            {
                    //                ImGui::SetItemDefaultFocus();
                    //            }
                    //        }

                    //        ImGui::EndCombo();
                    //    }

                    //    ImGui::PopID();

                    //    if (selectedIndex != currentShaderType)
                    //    {
                    //        materials[i].shader = static_cast<Assets::Shader>(selectedIndex);
                    //    }
                    //}

                    ImGui::EndTable();
                }
            }
        }
    }
}
