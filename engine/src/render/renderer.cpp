#include "render/renderer.h"

namespace DF::Render {
    Renderer::Renderer()
    {
        m_window.init();

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << '\n';
        }
    }

    void Renderer::render()
    {
        m_window.update();
    }
}
