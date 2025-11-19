#include <iostream>

#include <fmt/format.h>

#include "ui_debug/world_outliner/world_outliner.hpp"
#include "transform.hpp"
#include "metadata.hpp"
#include "model.hpp"
#include "point_light.hpp"
#include "core/engine.hpp"
#include "world/world.hpp"

#include "components/metadata.hpp"
#include "components/transform_matrix.hpp"
#include "components/model.hpp"
#include "components/point_light.hpp"

//static const std::array vertices{
//    // transforms            |  normals           |  texture coords
//
//    /* 0 */    0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f,  // Front
//    /* 1 */    0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
//    /* 2 */   -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
//    /* 3 */   -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
//
//    /* 4 */   -0.5f,  0.5f, -0.5f,    0.0f, 0.0f,-1.0f,    1.0f, 1.0f,  // Back
//    /* 5 */   -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,-1.0f,    1.0f, 0.0f,
//    /* 6 */    0.5f,  0.5f, -0.5f,    0.0f, 0.0f,-1.0f,    0.0f, 1.0f,
//    /* 7 */    0.5f, -0.5f, -0.5f,    0.0f, 0.0f,-1.0f,    0.0f, 0.0f,
//
//    /* 8 */    0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,  // Right
//    /* 9 */    0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
//    /* 10 */   0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
//    /* 11 */   0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
//
//    /* 12 */  -0.5f,  0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,    1.0f, 1.0f,  // Left
//    /* 13 */  -0.5f, -0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
//    /* 14 */  -0.5f,  0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
//    /* 15 */  -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
//
//    /* 16 */   0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,  // Top
//    /* 17 */  -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
//    /* 18 */   0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
//    /* 19 */  -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
//
//    /* 20 */   0.5f, -0.5f,  0.5f,    0.0f,-1.0f, 0.0f,    1.0f, 1.0f,  // Bottom
//    /* 21 */  -0.5f, -0.5f,  0.5f,    0.0f,-1.0f, 0.0f,    1.0f, 0.0f,
//    /* 22 */   0.5f, -0.5f, -0.5f,    0.0f,-1.0f, 0.0f,    0.0f, 1.0f,
//    /* 23 */  -0.5f, -0.5f, -0.5f,    0.0f,-1.0f, 0.0f,    0.0f, 0.0f,
//};
//static const std::array indices{
//    //  triangle 1  |  triangle 2
//         0, 1, 2,       1, 3, 2,       // Front
//         7, 6, 4,       7, 5, 4,       // Back
//
//         8, 9, 10,      10, 9, 11,     // Right
//         12, 13, 14,    14, 13, 15,    // Left
//
//         16, 18, 19,    16, 17, 19,    // Top
//         20, 22, 23,    20, 23, 21     // Bottom
//};

namespace DF::UI::Debug
{
    WorldOutliner::WorldOutliner() noexcept
    {
        m_world = Engine::getWorld();
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
            object.addComponent(Components::Model{ "../../resources/models/backpack/backpack.obj" });

            std::string newName{ objectName };

            if (objectName[0] == '\0')
            {
                newName = !counter ? "Object" : fmt::format("Object ({})", counter);
                ++counter;
            }

            object.addComponent(DF::Components::Metadata{ .name{ newName } });

            m_world->spawnObject(object);
        }

        if (ImGui::Button("Spawn Point Light"))
        {
            Object object{ m_world->createObject() };

            object.addComponent(Components::Transform{ .scale{ 0.2, 0.2, 0.2 } });
            object.addComponent(Components::PointLight{});

            Components::Model model{ "../../resources/models/cube/cube.obj" };
            model.materialOverrides[0] = DF::Assets::Material
            {
                .diffuse{ "../../resources/images/matrix.jpg" },
                .shader{ DF::Assets::Shader::UNLIT },
            };
            object.addComponent(model);

            object.addComponent(DF::Components::Metadata{ .name{ "Point Light" } });

            m_world->spawnObject(object);
        }

        if (ImGui::BeginChild("##objects_list", ImVec2(0, 300.0f), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened | ImGuiChildFlags_ResizeY))
        {
            ImGuiListClipper clipper{};
            clipper.Begin(static_cast<int>(objects.size()));

            static std::vector<bool> selection{};
            selection.resize(objects.size());

            static int selectedIndex{ -1 };

            while (clipper.Step())
            {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
                {
                    auto& obj = objects[i];
                    auto& metadata = obj.getComponent<Components::Metadata>();

                    ImGui::PushID(i);

                    if (ImGui::Selectable(metadata.name.c_str(), selection[i]))
                    {
                        if (selectedIndex != i && ImGui::IsItemFocused())
                        {
                            if (selectedIndex >= 0)
                            {
                                selection[selectedIndex] = false;
                            }

                            selection[i] = true;
                            selectedIndex = i;
                            m_selectedObjectId = objects[i].getId();
                        }
                    }

                    ImGui::PopID();
                }
            }
        }

        ImGui::EndChild();

        ImGui::Text("Components");

        if (ImGui::BeginChild("##components_list", ImVec2(0, 300.0f), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY))
        {
            if (m_selectedObjectId)
            {
                Object object{ m_world->getObject(m_selectedObjectId) };

                auto& metadata{ object.getComponent<Components::Metadata>() };

                ImGui::Text(metadata.name.c_str());
                ImGui::Separator();

                m_world->getObject(m_selectedObjectId).forEachComponent(
                    [this, &object, &metadata](Object::Id id)
                    {
                        if (m_world->isComponentType<Components::Metadata>(id))
                        {
                            Metadata::render(metadata);
                        }

                        if (m_world->isComponentType<Components::Transform>(id))
                        {
                            auto transform{ object.getComponent<Components::Transform>() };

                            Transform::render(transform);
                        }

                        if (m_world->isComponentType<Components::PointLight>(id))
                        {
                            auto& pointLight{ object.getComponent<Components::PointLight>() };

                            PointLight::render(pointLight);
                        }

                        if (m_world->isComponentType<Components::Model>(id))
                        {
                            auto& model{ object.getComponent<Components::Model>() };

                            Model::render(model);
                        }
                    }
                );
            }
        }

        ImGui::EndChild();
    }
}
