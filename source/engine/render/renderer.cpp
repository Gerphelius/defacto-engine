#include <gl/glew.h>
#include <iostream>

#include "platform/platform_window.hpp"

namespace DF::RENDER
{
const char* vert = R"(
    #version 330 core

    layout(location = 0) in vec3 aPos;

    void main()
    {
        gl_Position = vec4(aPos, 1.0f);
    }
)";

const char* frag = R"(
    #version 330 core

    out vec4 FragColor;

    uniform vec4 uColor;

    void main()
    {
        FragColor = uColor;
    }
)";

GLuint CompileShader(const char* source, GLenum type)
{
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(shader, 512, 0, infoLog);

        std::cout << type << " shader error: " << infoLog << '\n';
    }

    return shader;
}

GLuint shaderProgram;

static PLATFORM::Window* g_window;

void Initialize(PLATFORM::Window* window)
{
    // Disable depth testing for UI rendering so depth values do not affect
    // draw order. UI elements are rendered strictly in the order of glDraw calls.
    // glEnable(GL_DEPTH_TEST);

    g_window = window;

    shaderProgram         = glCreateProgram();
    GLuint vertexShader   = CompileShader(vert, GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(frag, GL_FRAGMENT_SHADER);

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);

        std::cout << "Shader program error: " << infoLog << '\n';
    }

    // Need to detach shader before deletion because the driver
    // won't free GPU memory until the shader program is deleted
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDeleteShader.xhtml
    glDetachShader(shaderProgram, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(fragmentShader);
}

void BeginFrame()
{
    PLATFORM::Size fbSize = GetFramebufferSize(g_window);
    glViewport(0, 0, fbSize.width, fbSize.height);

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void EndFrame()
{
    PLATFORM::SwapBuffers(g_window);
}

float Map(float value, float low1, float high1, float low2, float high2)
{
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

struct Size
{
    float width;
    float height;
};

struct Position
{
    float x;
    float y;
};

struct Color
{
    float r;
    float g;
    float b;
    float a;
};

void DrawQuad(Position pos, Size size, Color color)
{
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    float normX = Map(pos.x, (float)viewport[0], (float)viewport[2], -1.0f, 1.0f);
    float normY = Map(pos.y, (float)viewport[1], (float)viewport[3], -1.0f, 1.0f);
    float normW = Map(pos.x + size.width, (float)viewport[0], (float)viewport[2], -1.0f, 1.0f);
    float normH = Map(pos.y + size.height, (float)viewport[1], (float)viewport[3], -1.0f, 1.0f);

    float vertices[] {
        normX, -normY, 0.0f, // top left
        normW, -normY, 0.0f, // top right
        normW, -normH, 0.0f, // bottom right
        normX, -normH, 0.0f, // bottom left
    };
    int indices[] { 0, 1, 3, 1, 2, 3 };

    unsigned int vbo;
    glGenBuffers(1, &vbo);

    unsigned int ebo;
    glGenBuffers(1, &ebo);

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);

    glUseProgram(shaderProgram);

    float r = Map(color.r, 0.0f, 255.0f, 0.0f, 1.0f);
    float g = Map(color.g, 0.0f, 255.0f, 0.0f, 1.0f);
    float b = Map(color.b, 0.0f, 255.0f, 0.0f, 1.0f);
    float a = Map(color.a, 0.0f, 255.0f, 0.0f, 1.0f);

    glUniform4f(glGetUniformLocation(shaderProgram, "uColor"), r, g, b, a);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDeleteBuffers(vbo, &vbo);
    glDeleteBuffers(ebo, &ebo);
    glDeleteVertexArrays(vao, &vao);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // namespace DF::RENDER
