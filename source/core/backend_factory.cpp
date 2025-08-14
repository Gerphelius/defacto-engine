#include "core/backend_factory.hpp"
#include "backend/glfw/window.hpp"
#include "backend/glfw/input.hpp"

namespace DF::Core
{
    std::unique_ptr<Render::Window> BackendFactory::createWindow(int width, int height, std::string_view title)
    {
        return std::make_unique<Backend::WindowGLFW>(width, height, title);
    }

    std::unique_ptr<Input> BackendFactory::createInput(Render::Window* window)
    {
        auto input{ std::make_unique<Backend::GLFWInput>() };

        static_cast<Backend::WindowGLFW*>(window)->connectInput(input.get());

        return input;
    }
}
