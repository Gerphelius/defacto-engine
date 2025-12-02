#include <iostream>

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <string>

#include <fmt/format.h>

#include "world_outliner.hpp"
#include "component_widget.hpp"
#include "transform.hpp"
#include "metadata.hpp"
#include "model.hpp"
#include "point_light.hpp"
#include "camera.hpp"

#include "core/engine.hpp"
#include "world/world.hpp"

#include "components/metadata.hpp"
#include "components/transform_matrix.hpp"
#include "components/model.hpp"
#include "components/point_light.hpp"
#include "components/camera.hpp"

namespace DF::UI::Debug
{
    static std::unordered_map<Object::Id, std::unique_ptr<IComponentWidget>> s_componentToWidget{};
    /*
    * @brief Set of disabled components so user cannot delete them on components add or remove,
    * because they are required for object to be spawned in the world
    */
    static std::unordered_set<Object::Id> s_disabledComponents{};

    WorldOutliner::WorldOutliner() noexcept
    {
        m_world = Engine::getWorld();

        Object::Id transformId{ m_world->getComponentId<Components::Transform>() };
        Object::Id modelId{ m_world->getComponentId<Components::Model>() };
        Object::Id metadataId{ m_world->getComponentId<Components::Metadata>() };

        s_componentToWidget[transformId] = std::make_unique<Transform>();
        s_componentToWidget[modelId] = std::make_unique<Model>();
        s_componentToWidget[metadataId] = std::make_unique<Metadata>();
        s_componentToWidget[m_world->getComponentId<Components::PointLight>()] = std::make_unique<PointLight>();
        s_componentToWidget[m_world->getComponentId<Components::Camera>()] = std::make_unique<Camera>();

        s_disabledComponents.insert(transformId);
        s_disabledComponents.insert(modelId);
        s_disabledComponents.insert(metadataId);
    }

    const std::string& WorldOutliner::getName() const
    {
        return m_name;
    }

    void WorldOutliner::render()
    {
        std::vector<Object> objects{};
        m_world->forEach<Object, Components::TransformMatrix>(
            [&objects](auto obj, auto&)
            {
                objects.push_back(obj);
            }
        );

        ImGui::Text("Objects");

        static int counter{};
        static char objectName[128]{};
        ImGui::InputTextWithHint("##", "Enter object name", objectName, IM_ARRAYSIZE(objectName));
        ImGui::SameLine();

        if (ImGui::Button("Spawn"))
        {
            Object object{ m_world->createObject() };

            object.addComponent(Components::Transform{});
            object.addComponent(Components::Model{ "cube" });

            std::string newName{ objectName };

            if (objectName[0] == '\0')
            {
                newName = !counter ? "Object" : fmt::format("Object ({})", counter);
                ++counter;
            }

            object.addComponent(DF::Components::Metadata{ .name{ newName } });

            m_world->spawnObject(object);
        }

        static int selectedIndex{ -1 };

        if (ImGui::BeginChild("##objects_list", ImVec2(0, 300.0f), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened | ImGuiChildFlags_ResizeY))
        {
            if (ImGui::BeginTable("##", 2, ImGuiTableFlags_RowBg))
            {
                ImGuiListClipper clipper{};
                clipper.Begin(static_cast<int>(objects.size()));

                ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed);

                while (clipper.Step())
                {
                    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        auto& obj = objects[i];
                        auto& metadata = obj.getComponent<Components::Metadata>();

                        ImGui::PushID(i);
                        ImGui::SetNextItemAllowOverlap();

                        if (ImGui::Selectable(metadata.name.c_str(), selectedIndex == i, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap))
                        {
                            if (selectedIndex != i && ImGui::IsItemFocused())
                            {
                                selectedIndex = i;
                            }
                        }

                        ImGui::TableNextColumn();

                        if (ImGui::SmallButton("Delete"))
                        {
                            obj.destroy();
                            selectedIndex = -1;
                        }

                        ImGui::PopID();
                    }
                }

                ImGui::EndTable();
            }

        }

        ImGui::EndChild();

        renderComponentsList(selectedIndex, objects);
    }

    void WorldOutliner::renderComponentsList(int selectedIndex, std::vector<Object>& objects)
    {
        ImGui::Text("Components");

        if (ImGui::BeginChild("##components_list", ImVec2(0, 300.0f), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY))
        {
            if (selectedIndex >= 0)
            {
                Object object{ objects[selectedIndex] };

                static bool addNew{};
                static std::unordered_set<Object::Id> itemsToAdd{};
                static std::unordered_set<Object::Id> itemsToRemove{};

                if (ImGui::BeginTable("##head_spacer", 2))
                {
                    ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    auto& metadata{ object.getComponent<Components::Metadata>() };
                    ImGui::Text(metadata.name.c_str());
                    ImGui::TableNextColumn();

                    if (ImGui::SmallButton(addNew ? "Done" : "Add Components"))
                    {
                        if (addNew)
                        {
                            for (const auto item : itemsToAdd)
                            {
                                object.raw().add(item);
                            }

                            for (const auto item : itemsToRemove)
                            {
                                if (object.raw().has(item))
                                {
                                    object.raw().remove(item);
                                }
                            }

                            itemsToAdd.clear();
                            itemsToRemove.clear();
                        }

                        addNew = !addNew;
                    }

                    ImGui::EndTable();
                }

                ImGui::Separator();

                std::unordered_set<Object::Id> objectComponents{};
                std::unordered_set<Object::Id> availableComponents{};

                for (const auto& item : s_componentToWidget)
                {
                    if (object.raw().has(item.first) || itemsToAdd.contains(item.first))
                    {
                        objectComponents.insert(item.first);
                    }
                }

                for (const auto& item : s_componentToWidget)
                {
                    if (!objectComponents.contains(item.first) || itemsToRemove.contains(item.first))
                    {
                        availableComponents.insert(item.first);
                    }
                }

                if (ImGui::BeginTable("##block_separation", addNew ? 3 : 1))
                {
                    if (addNew)
                    {
                        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch);
                    }

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();

                    static std::unordered_set<Object::Id> selectedToRemove{};

                    if (ImGui::BeginTable("##current_components", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersOuterV))
                    {
                        for (const auto& component : objectComponents)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();

                            const std::string& componentName{ s_componentToWidget[component]->getName() };

                            if (addNew)
                            {
                                int disabledFlag = s_disabledComponents.contains(component) ? ImGuiSelectableFlags_Disabled : ImGuiSelectableFlags_None;

                                if (ImGui::Selectable(componentName.c_str(), selectedToRemove.contains(component), disabledFlag))
                                {
                                    if (selectedToRemove.contains(component))
                                    {
                                        selectedToRemove.erase(component);
                                    }
                                    else
                                    {
                                        selectedToRemove.insert(component);
                                    }
                                }
                            }
                            else
                            {
                                s_componentToWidget[component]->render(object.raw().get_mut(component));
                            }
                        }

                        ImGui::EndTable();
                    }

                    if (addNew)
                    {
                        static std::unordered_set<Object::Id> selectedToAdd{};

                        ImGui::TableNextColumn();

                        ImGui::BeginGroup();
                        {
                            ImGui::BeginGroup();

                            if (ImGui::Button("<"))
                            {
                                for (const auto& item : selectedToAdd)
                                {
                                    itemsToRemove.erase(item);
                                }

                                itemsToAdd.merge(selectedToAdd);
                            }

                            if (ImGui::Button(">"))
                            {
                                for (const auto& item : selectedToRemove)
                                {
                                    itemsToAdd.erase(item);
                                }

                                itemsToRemove.merge(selectedToRemove);
                            }

                            ImGui::EndGroup();
                        }

                        ImGui::TableNextColumn();

                        if (ImGui::BeginTable("##new_components", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersOuterV))
                        {
                            for (const auto& component : availableComponents)
                            {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();

                                const std::string& componentName{ s_componentToWidget[component]->getName() };

                                if (ImGui::Selectable(componentName.c_str(), selectedToAdd.contains(component)))
                                {
                                    if (selectedToAdd.contains(component))
                                    {
                                        selectedToAdd.erase(component);
                                    }
                                    else
                                    {
                                        selectedToAdd.insert(component);
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
        }

        ImGui::EndChild();
    }
}
