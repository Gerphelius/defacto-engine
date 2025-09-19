#include <iostream>
#include <string>
#include <array>

#include <fmt/format.h>

#include "core/engine.hpp"
#include "world/world.hpp"
#include "input/input.hpp"
#include "math/math.hpp"
#include "components/transform.hpp"
#include "components/model.hpp"
#include "components/metadata.hpp"


/* TODO:
* # Create API for getting references to window, input etc. (service locator)
* # Make a UI to list all spawned objects in the world:
*  -- Create base UI class/struct with some methods like render;
*  -- Create a world objects list (WOL) UI inherited from base UI;
*  -- Register WOL UI inside debug UI system;
*  -- Create some kind of callbacks or basic observable to decouple UI from systems like renderer etc;
*  -- Add an ability to create new object, add components, spawn object in the world.
*
* # Move default camera logic to world class and create it on init.
* # Create asset manager and refactor path to use std::filesystem::path instead of relative.
* # CMake - check path definition to assets, so during build on other machines assets are properly copied
*   to where .exe lives and are loaded to the game.
*   (ref https://youtu.be/8bNsCBkX_bA?t=439)
*/

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

int main()
{
    DF::Engine::init();

    ///////////////////////// ECS TEST /////////////////////////

    //const auto world{ DF::Engine::getWorld() };

    //for (int i{}; i < 100; ++i)
    //{
    //    for (int j{}; j < 100; ++j)
    //    {
    //        auto object{ world->createObject() };

    //        DF::Components::Transform transformComp{};
    //        DF::Components::Model modelComp{ std::make_shared<DF::Assets::Mesh>(vertices, indices) };

    //        transformComp.position.x = i * 2.0f;
    //        transformComp.position.z = j * 2.0f;

    //        object.addComponent(transformComp);
    //        object.addComponent(modelComp);
    //        object.addComponent(DF::Components::Metadata{ .name{ fmt::format("Cube {}", 100 * i + j) } });

    //        world->spawnObject(object);
    //    }
    //}

    ////////////////////////////////////////////////////////////

    DF::Engine::run();

    return 0;
}
