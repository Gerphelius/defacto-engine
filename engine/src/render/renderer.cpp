#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "render/renderer.h"
#include "render/window.h"

namespace DF::Render {
    Renderer::Renderer()
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << '\n';
        }
    }

    void Renderer::render()
    {

    }
}
