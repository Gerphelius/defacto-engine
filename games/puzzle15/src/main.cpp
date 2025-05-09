#include <iostream>
#include <string>
#include <array>

#include "df/core/engine.hpp"
#include "df/core/world.hpp"
#include "df/input/input_system.hpp"
#include "df/utils/math.hpp"


/* TODO:
* 
* Create asset manager with primitives;
* Create mesh class that will create VBO, VAO, EBO from vertices/indices;
* When creating primitive mesh class, create it once and store it in asset manager as shared ptr;
* Create mesh component that will hold pointer to a mesh class;
* Iterate mesh components inside renderer and bind mesh class VAO;
* 
*/

static const std::array vertices {
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
static const std::array indices {
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
    DF::Core::Engine eng{};

    ///////////////////////// ECS TEST /////////////////////////

    const auto world{ eng.getWorld() };

    auto object{ world->createObject() };

    DF::Core::Transform transformComp{};
    DF::Core::Model modelComp{ std::make_shared<DF::Assets::Mesh>(vertices, indices) };

    world->addTransform(object, transformComp);
    world->addModel(object, modelComp);

    auto inputSystem{ DF::Input::getInputSystem() };

    inputSystem->onKeyPress(
        DF::Input::Key::UP,
        DF::Input::KeyEvent::HOLD,
        [object, &world, &eng]() mutable {
            auto& transform{ world->getTransform(object) };

            transform.m_position.z += (5.0f * eng.getDeltaTime());
        }
    );
    inputSystem->onKeyPress(
        DF::Input::Key::DOWN,
        DF::Input::KeyEvent::HOLD,
        [object, &world, &eng]() mutable {
            auto& transform{ world->getTransform(object) };

            transform.m_position.z -= (5.0f * eng.getDeltaTime());
        }
    );
    inputSystem->onKeyPress(
        DF::Input::Key::RIGHT,
        DF::Input::KeyEvent::HOLD,
        [object, &world, &eng]() mutable {
            auto& transform{ world->getTransform(object) };

            transform.m_position.x += (5.0f * eng.getDeltaTime());
        }
    );
    inputSystem->onKeyPress(
        DF::Input::Key::LEFT,
        DF::Input::KeyEvent::HOLD,
        [object, &world, &eng]() mutable {
            auto& transform{ world->getTransform(object) };

            transform.m_position.x -= (5.0f * eng.getDeltaTime());
        }
    );

    ////////////////////////////////////////////////////////////

    eng.run();

    return 0;
}
