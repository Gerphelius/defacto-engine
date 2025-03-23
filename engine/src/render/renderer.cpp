#include <iostream>

#include "render/renderer.h"
#include "render/window.h"
#include "utils/file_io.h"

namespace DF::Render {
    Renderer::Renderer()
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << '\n';
        }

        const float vertices[] = {
             0.5f,  0.5f, 0.0f,     // top right
             0.5f, -0.5f, 0.0f,     // bottom right
            -0.5f, -0.5f, 0.0f,     // bottom left
            -0.5f,  0.5f, 0.0f      // top left
        };
        const int indices[] = {
            0, 1, 3,    // first triangle
            1, 2, 3     // second triangle
        };

        GLuint vbo{};
        glGenBuffers(1, &vbo);

        GLuint ebo{};
        glGenBuffers(1, &ebo);

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        GLuint vertexShader{ glCreateShader(GL_VERTEX_SHADER) };
        auto shaderSource{ File::read("../../assets/shaders/base.vert.glsl") };
        auto ss{ shaderSource.c_str() };

        glShaderSource(vertexShader, 1, &ss, NULL);
        glCompileShader(vertexShader);

        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);

            std::cout << "Vertex shader error: " << infoLog << '\n';
        }

        GLuint fragmentShader{ glCreateShader(GL_FRAGMENT_SHADER) };
        shaderSource = File::read("../../assets/shaders/base.frag.glsl");
        ss = shaderSource.c_str();

        glShaderSource(fragmentShader, 1, &ss, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);

            std::cout << "Fragment shader error: " << infoLog << '\n';
        }

        m_shaderProgram = glCreateProgram();
        glAttachShader(m_shaderProgram, vertexShader);
        glAttachShader(m_shaderProgram, fragmentShader);
        glLinkProgram(m_shaderProgram);

        glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);

        if (!success)
        {
            glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);

            std::cout << "Shader program error: " << infoLog << '\n';
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
        glEnableVertexAttribArray(0);

        // Unbind current vao, vbo and ebo respectively
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glUseProgram(m_shaderProgram);
        glBindVertexArray(m_vao);
    }

    void Renderer::render()
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    void Renderer::setDrawMode(DrawMode mode)
    {
        m_drawMode = mode;
        glPolygonMode(GL_FRONT_AND_BACK, mode == DrawMode::fill ? GL_FILL : GL_LINE);
    }
}
