#include "core/world.hpp"
#include "core/engine.hpp"
#include "core/service_locator.hpp"
#include "input/input.hpp"

namespace DF::Core
{
    World::World() noexcept
    {
        spawnDefaultCamera();
    }

    void World::spawnDefaultCamera()
    {
        constexpr float rotationSpeed{ 0.3f };
        constexpr float maxCameraSpeed{ 40.0f };
        constexpr float normalCameraSpeed{ 10.0f };
        static float cameraSpeed{ normalCameraSpeed };

        auto* input{ ServiceLocator::getService<Input>() };
        auto defaultCamera{ createObject() };

        addComponent<Components::Camera>(defaultCamera, Components::Camera{ .active{ true } });
        addComponent<Components::Transform>(defaultCamera, Components::Transform{});

        input->onKeyPress(
            Input::Key::W,
            Input::KeyEvent::HOLD,
            [this, defaultCamera]() {
                auto transform{ getComponent<Components::Transform>(defaultCamera) };

                if (!transform) return;

                transform->addPosition(transform->getForwardVector() * cameraSpeed * Engine::getDeltaTime());
            }
        );
        input->onKeyPress(
            Input::Key::S,
            Input::KeyEvent::HOLD,
            [this, defaultCamera]() {
                auto transform{ getComponent<Components::Transform>(defaultCamera) };

                if (!transform) return;

                transform->addPosition(-transform->getForwardVector() * cameraSpeed * Engine::getDeltaTime());
            }
        );
        input->onKeyPress(
            Input::Key::D,
            Input::KeyEvent::HOLD,
            [this, defaultCamera]() {
                auto transform{ getComponent<Components::Transform>(defaultCamera) };

                if (!transform) return;

                transform->addPosition(transform->getRightVector() * cameraSpeed * Engine::getDeltaTime());
            }
        );
        input->onKeyPress(
            Input::Key::A,
            Input::KeyEvent::HOLD,
            [this, defaultCamera]() {
                auto transform{ getComponent<Components::Transform>(defaultCamera) };

                if (!transform) return;

                transform->addPosition(-transform->getRightVector() * cameraSpeed * Engine::getDeltaTime());
            }
        );

        input->onKeyPress(
            Input::Key::SHIFT_L,
            Input::KeyEvent::PRESS,
            [maxCameraSpeed]() mutable {
                cameraSpeed = maxCameraSpeed;
            }
        );
        input->onKeyPress(
            Input::Key::SHIFT_L,
            Input::KeyEvent::RELEASE,
            [normalCameraSpeed]() mutable {
                cameraSpeed = normalCameraSpeed;
            }
        );

        input->onMouseMove(
            [this, rotationSpeed, defaultCamera, input](Math::vec2 pos) {
                static Math::vec2 s_lastPos{};

                if (input->mouseKeyPressed(Input::MouseKey::RIGHT))
                {
                    Math::vec2 currentPos{ pos - s_lastPos };

                    auto transform{ getComponent<Components::Transform>(defaultCamera) };

                    if (!transform) return;

                    auto newRotation{ transform->getRotation() - Math::vec3(currentPos.x * -rotationSpeed, currentPos.y * rotationSpeed, 0.0) };
                    newRotation.y = std::clamp(newRotation.y, -89.0f, 89.0f);

                    transform->setRotation(newRotation);
                }

                s_lastPos = pos;
            }
        );
    }
}
