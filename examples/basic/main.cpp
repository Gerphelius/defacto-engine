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
#include "components/point_light.hpp"


/* TODO:
* # Create API for getting references to window, input etc. (service locator)  +
* # Make a UI to list all spawned objects in the world:
*  -- Create base UI class/struct with some methods like render;  +
*  -- Create a world objects list (WOL) UI inherited from base UI;  +
*  -- Add an ability to spawn object in the world,  +
*
* # Move default camera logic to world class and create it on init.  +
* # Create asset manager and refactor path to use std::filesystem::path instead of relative.
* # CMake - check path definition to assets, so during build on other machines assets are properly copied
*   to where .exe lives and are loaded to the game.
*   (ref https://youtu.be/8bNsCBkX_bA?t=439)
*/

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
