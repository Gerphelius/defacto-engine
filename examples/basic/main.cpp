#include <iostream>
#include <string>
#include <array>
#include <unordered_map>

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
*   CMake - check path definition to assets, so during build on other machines assets are properly copied
*   to where .exe lives and are loaded to the game.
*   (ref https://youtu.be/8bNsCBkX_bA?t=439)
*/

int main()
{
    DF::Engine::init();

    ///////////////////////// ECS TEST /////////////////////////

    DF::World* world{ DF::Engine::getWorld() };

    //DF::Object obj{ world->createObject() };

    //obj.addComponent(DF::Components::Transform{});
    //obj.addComponent(DF::Components::Model{ "../../resources/models/cube/multimat.obj" });
    //obj.addComponent(DF::Components::Metadata{ "No naterial cube", "Simple cubics" });

    //world->spawnObject(obj);

    //DF::Object obj1{ world->createObject() };

    //obj1.addComponent(DF::Components::Transform{});
    //obj1.addComponent(DF::Components::Model{ "../../resources/models/cube/new_cube.obj" });
    //obj1.addComponent(DF::Components::Metadata{ "Cube with one material" });

    //world->spawnObject(obj1);

    DF::Object obj2{ world->createObject() };

    DF::Assets::AssetManager::loadTexture("../../resources/images/container2.png");
    DF::Assets::AssetManager::loadTexture("../../resources/images/awesomeface.png");
    DF::Assets::AssetManager::loadTexture("../../resources/images/matrix.jpg");


    DF::Components::Model model{ "../../resources/models/cube/multimat.obj" };
    model.materialOverrides[0] = DF::Assets::Material
    {
        .diffuse{ "../../resources/images/container2.png" },
        .shader{ DF::Assets::Shader::PHONG },
    };
    model.materialOverrides[1] = DF::Assets::Material
    {
        .diffuse{ "../../resources/images/awesomeface.png" },
        .shader{ DF::Assets::Shader::PHONG },
    };
    model.materialOverrides[2] = DF::Assets::Material
    {
        .diffuse{ "../../resources/images/matrix.jpg" },
        .shader{ DF::Assets::Shader::PHONG },
    };
    obj2.addComponent(model);

    obj2.addComponent(DF::Components::Transform{});
    obj2.addComponent(DF::Components::Metadata{ "Multimat model" });

    world->spawnObject(obj2);

    ////////////////////////////////////////////////////////////

    DF::Engine::run();

    return 0;
}
