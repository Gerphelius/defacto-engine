#pragma once

#include <memory>
#include <string_view>

namespace DF::Render
{
    class Window;
}
namespace DF
{
    class Input;
}

namespace DF::Core
{
    class BackendFactory final
    {
    public:
        static std::unique_ptr<Render::Window> createWindow(int width, int height, std::string_view title);

        static std::unique_ptr<Input> createInput(Render::Window* window);

    private:
        BackendFactory() = delete;

        friend class Engine;
    };
}
