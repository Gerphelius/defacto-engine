#pragma once

#include <string>
#include <unordered_map>
#include <typeindex>

#include <imgui.h>
#include <fmt/format.h>

#include <components/point_light.hpp>
#include <components/camera.hpp>

#include "ui_debug/widget.hpp"

namespace Examples::Basic
{
    static const std::unordered_map<std::string, std::type_index> s_components
    {
        { "Point Light", typeid(DF::Components::PointLight) },
        { "Camera",      typeid(DF::Components::Camera) },
    };

    class Table : public DF::UI::Debug::IWidget
    {
    public:
        const std::string& getName() const override
        {
            return m_name;
        }

        void render() override
        {
            static std::unordered_map<std::string, std::type_index> list
            {
                { "Transform", typeid(DF::Components::PointLight) },
                { "Model",     typeid(DF::Components::PointLight) },
                { "Metadata",  typeid(DF::Components::PointLight) },
            };
            std::unordered_map<std::string, std::type_index> avaliableComponents{};

            for (const auto& component : s_components)
            {
                if (!list.contains(component.first))
                {
                    avaliableComponents.insert(component);
                }
            }

            static bool addNew{};

            if (ImGui::Button(addNew ? "Done" : "Add component"))
            {
                addNew = !addNew;
            }

            if (ImGui::BeginTable("##", addNew ? 3 : 1))
            {
                if (addNew)
                {
                    ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch);
                }

                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                static std::unordered_map<std::string, std::type_index> itemsToRemove{};

                if (ImGui::BeginTable("main", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersOuterV))
                {
                    for (const auto& item : list)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        if (addNew)
                        {
                            int disabledFlag = !s_components.contains(item.first) ? ImGuiSelectableFlags_Disabled : ImGuiSelectableFlags_None;

                            if (ImGui::Selectable(item.first.c_str(), itemsToRemove.contains(item.first), disabledFlag))
                            {
                                if (itemsToRemove.contains(item.first))
                                {
                                    itemsToRemove.erase(item.first);
                                }
                                else
                                {
                                    itemsToRemove.insert(item);
                                }
                            }
                        }
                        else
                        {
                            if (ImGui::CollapsingHeader(item.first.c_str(), ImGuiTreeNodeFlags_None))
                            {
                                ImGui::PushID(item.first.c_str());
                                ImGui::Button("Test");
                                ImGui::PopID();
                            }
                        }
                    }

                    ImGui::EndTable();
                }

                if (addNew)
                {
                    static std::unordered_map<std::string, std::type_index> itemsToAdd{};

                    ImGui::TableNextColumn();

                    ImGui::BeginGroup();
                    {
                        ImGui::BeginGroup();

                        if (ImGui::Button("<"))
                        {
                            list.merge(itemsToAdd);
                        }

                        if (ImGui::Button(">"))
                        {
                            for (const auto& item : itemsToRemove)
                            {
                                list.erase(item.first);
                            }
                        }

                        ImGui::EndGroup();
                    }

                    ImGui::TableNextColumn();

                    if (ImGui::BeginTable("aux", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersOuterV))
                    {
                        for (const auto& component : avaliableComponents)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();

                            if (ImGui::Selectable(component.first.c_str(), itemsToAdd.contains(component.first)))
                            {
                                if (itemsToAdd.contains(component.first))
                                {
                                    itemsToAdd.erase(component.first);
                                }
                                else
                                {
                                    itemsToAdd.insert(component);
                                }
                            }
                        }

                        ImGui::EndTable();
                    }

                    ImGui::EndGroup();
                }

                ImGui::EndTable();
            }
        }

    private:
        std::string m_name{ "Table" };
    };
}
