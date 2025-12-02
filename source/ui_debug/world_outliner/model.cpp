#include <string>

#include <imgui.h>
#include <fmt/format.h>

#include "model.hpp"
#include "assets/asset_manager.hpp"
#include "components/model.hpp"

namespace ImGui
{
    // ImGui::InputText() with std::string
    // Because text input needs dynamic resizing, we need to setup a callback to grow the capacity
    IMGUI_API bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
}

namespace DF::UI::Debug
{
    const std::string& Model::getName() const
    {
        return m_name;
    }

    void Model::render(void* component)
    {
        if (ImGui::CollapsingHeader(m_name.c_str(), ImGuiTreeNodeFlags_None))
        {
            auto* modelComp{ static_cast<Components::Model*>(component) };
            std::string path{ modelComp->model };
            static std::string newPath{ path };

            if (ImGui::BeginTable("##properties", 2, ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 2.0f);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Path");
                ImGui::TableNextColumn();

                ImGui::InputText("##", &newPath, 128);
                ImGui::SameLine();

                ImGui::EndTable();
            }

            ImGui::SeparatorText("Materials");

            Assets::Model* model{ Assets::AssetManager::getModel(path) };
            auto& materials{ model->getMaterials() };
            static Assets::MaterialOverrides matOverrides{};
            static Assets::Model* prevModel{ model };

            // TODO: move it after apply button to reset overrides
            if (prevModel != model)
            {
                prevModel = model;
                newPath = path;
                matOverrides.clear();
            }

            for (int i{}; i < materials.size(); ++i)
            {
                if (ImGui::TreeNode(fmt::format("{}: {}", i, *materials[i].name).c_str()))
                {
                    const auto& matOverride{ modelComp->materialOverrides.find(i) };
                    const bool hasOverride{ matOverride != modelComp->materialOverrides.end() };

                    matOverrides[i].diffuse = matOverrides[i].diffuse.value_or(hasOverride ? matOverride->second.diffuse.value_or(*materials[i].diffuse) : *materials[i].diffuse);
                    matOverrides[i].specular = matOverrides[i].specular.value_or(hasOverride ? matOverride->second.specular.value_or(*materials[i].specular) : *materials[i].specular);
                    matOverrides[i].shader = matOverrides[i].shader.value_or(hasOverride ? matOverride->second.shader.value_or(*materials[i].shader) : *materials[i].shader);

                    if (ImGui::BeginTable("##properties", 2, ImGuiTableFlags_Resizable))
                    {
                        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 2.0f);

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted("Diffuse");
                        ImGui::TableNextColumn();

                        ImGui::InputText("##diffuse", &matOverrides[i].diffuse.value(), 128);

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted("Specular");
                        ImGui::TableNextColumn();

                        ImGui::InputText("##specular", &matOverrides[i].specular.value(), 128);

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted("Shader");
                        ImGui::TableNextColumn();

                        const auto& shaders{ Assets::AssetManager::getShaderNames() };
                        int shaderType{ static_cast<int>(matOverrides[i].shader.value()) };
                        int selectedShaderType{ shaderType };
                        const char* selectedShaderName = shaders[selectedShaderType].c_str();

                        if (ImGui::BeginCombo("##shader", selectedShaderName))
                        {
                            for (int j{}; j < shaders.size(); ++j)
                            {
                                const bool is_selected = (j == selectedShaderType);

                                if (ImGui::Selectable(shaders[j].c_str(), is_selected))
                                {
                                    selectedShaderType = j;
                                }

                                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                                if (is_selected)
                                {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }

                            ImGui::EndCombo();
                        }

                        if (shaderType != selectedShaderType)
                        {
                            matOverrides[i].shader = static_cast<Assets::ShaderType>(selectedShaderType);
                        }

                        ImGui::EndTable();
                    }

                    ImGui::TreePop();
                }
            }

            ImVec2 buttonSize{ 100.0f, 0.0f };
            float padding{ 5.0f };

            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - buttonSize.x - padding);

            if (ImGui::Button("Apply changes", buttonSize))
            {
                if (Assets::AssetManager::loadModel(path))
                {
                    modelComp->model = newPath;
                }
                else
                {
                    newPath = modelComp->model;
                }

                /**
                * TODO: For now, if one model has more than one material applied and i.e. second material then overriten,
                *       if model has been changed to one that has less materials, redundant overrides will be saved to
                *       model modelComp->
                * Example: Have 2 models, first with 1 material, second with 3 materials. If second model was set in model component
                *          and material 2 or 3 has been overriten, when changin to first model, 2nd or 3rd override will be still
                *          present in material component overrides, even though model only has 1 material.
                */
                for (const auto& matOverride : matOverrides)
                {
                    modelComp->materialOverrides[matOverride.first].diffuse = matOverride.second.diffuse;
                    modelComp->materialOverrides[matOverride.first].specular = matOverride.second.specular;
                    modelComp->materialOverrides[matOverride.first].shader = matOverride.second.shader;
                }
            }
        }
    }
}
