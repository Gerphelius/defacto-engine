#pragma once

#include <functional>
#include <memory>

#include "render/renderer.h"
#include "input/input_manager.h"

namespace DF::Core
{
    class Engine final
    {
    private:
        Render::Renderer m_renderer{};
        std::shared_ptr<Input::IInputManager> m_inputManager{};
        bool m_running{ false };

    public:
        Engine();

        void run(std::function<void(double)> update);
    };
}
