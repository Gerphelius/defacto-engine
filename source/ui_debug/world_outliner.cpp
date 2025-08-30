#include <fmt/format.h>

#include "ui_debug/world_outliner.hpp"
#include "core/engine.hpp"
#include "world/world.hpp"
#include "components/metadata.hpp"
#include "components/transform_matrix.hpp"
#include "components/model.hpp"



static const std::array vertices{
    // transforms            |  normals           |  texture coords

    /* 0 */    0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f,  // Front
    /* 1 */    0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
    /* 2 */   -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
    /* 3 */   -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,

    /* 4 */   -0.5f,  0.5f, -0.5f,    0.0f, 0.0f,-1.0f,    1.0f, 1.0f,  // Back
    /* 5 */   -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,-1.0f,    1.0f, 0.0f,
    /* 6 */    0.5f,  0.5f, -0.5f,    0.0f, 0.0f,-1.0f,    0.0f, 1.0f,
    /* 7 */    0.5f, -0.5f, -0.5f,    0.0f, 0.0f,-1.0f,    0.0f, 0.0f,

    /* 8 */    0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,  // Right
    /* 9 */    0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
    /* 10 */   0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
    /* 11 */   0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,

    /* 12 */  -0.5f,  0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,    1.0f, 1.0f,  // Left
    /* 13 */  -0.5f, -0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
    /* 14 */  -0.5f,  0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
    /* 15 */  -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,

    /* 16 */   0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,  // Top
    /* 17 */  -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
    /* 18 */   0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
    /* 19 */  -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,

    /* 20 */   0.5f, -0.5f,  0.5f,    0.0f,-1.0f, 0.0f,    1.0f, 1.0f,  // Bottom
    /* 21 */  -0.5f, -0.5f,  0.5f,    0.0f,-1.0f, 0.0f,    1.0f, 0.0f,
    /* 22 */   0.5f, -0.5f, -0.5f,    0.0f,-1.0f, 0.0f,    0.0f, 1.0f,
    /* 23 */  -0.5f, -0.5f, -0.5f,    0.0f,-1.0f, 0.0f,    0.0f, 0.0f,
};
static const std::array indices{
    //  triangle 1  |  triangle 2
         0, 1, 2,       1, 3, 2,       // Front
         7, 6, 4,       7, 5, 4,       // Back

         8, 9, 10,      10, 9, 11,     // Right
         12, 13, 14,    14, 13, 15,    // Left

         16, 18, 19,    16, 17, 19,    // Top
         20, 22, 23,    20, 23, 21     // Bottom
};




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
        ImGui::BeginChild(ImGui::GetID(1), ImVec2(ImGui::GetContentRegionAvail().x, 200.0f));

        std::vector<Core::Object> objects{};
        m_world->forEachObject<Components::TransformMatrix>(
            [&objects](Core::Object& obj, const auto&) {
                objects.push_back(obj);
            }
        );

        if (ImGui::Button("Spawn Object"))
        {
            static int counter{};

            Core::Object object{ m_world->createObject() };

            m_world->addComponent(object, Components::Transform{});
            m_world->addComponent(object, Components::Model{ std::make_shared<DF::Assets::Mesh>(vertices, indices) });
            m_world->addComponent(object, DF::Components::Metadata{ .name{ fmt::format("New object {}", counter++) } });

            m_world->spawnObject(object);
        }

        ImGuiListClipper clipper{};
        clipper.Begin(static_cast<int>(objects.size()));

        while (clipper.Step()) {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                auto obj = objects[i];
                auto metadata = m_world->getComponent<Components::Metadata>(obj);

                std::string objectName{ metadata ? metadata->name.c_str() : "Unknown" };

                if (ImGui::CollapsingHeader(objectName.c_str(), ImGuiTreeNodeFlags_None))
                {
                    for (int i = 0; i < 5; i++)
                    {

                    }
                }
            }
        }

        ImGui::EndChild();
    }
}