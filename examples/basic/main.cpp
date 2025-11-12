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

    DF::Object obj{ world->createObject() };

    DF::Assets::AssetManager::loadTexture("../../resources/images/matrix.jpg");

    DF::Components::Model model{ "../../resources/models/cube/multimat.obj" };
    model.materialOverrides[0] = DF::Assets::Material
    {
        .diffuse{ "../../resources/images/matrix.jpg" },
        //.shader{ DF::Assets::Shader::UNLIT },
    };

    obj.addComponent(DF::Components::Transform{});
    obj.addComponent(model);
    obj.addComponent(DF::Components::Metadata{ "Multimat model" });

    world->spawnObject(obj);

    ////////////////////////////////////////////////////////////

    DF::Engine::run();

    return 0;
}
