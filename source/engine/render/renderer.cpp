#include <cstdint>
#include <gl/glew.h>
#include <iostream>
#include <fstream>
#include <string>

#include "platform/platform_window.hpp"

namespace DF::Render
{

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

static Platform::Window* g_window;

std::string ReadShaderFile(const std::string& path)
{
    std::ifstream file { path };

    if (!file)
    {
        std::cout << "Cannot read file: " << path << '\n';

        return "";
    }

    std::string line {};
    std::string content {};

    while (!file.eof())
    {
        std::getline(file, line);
        content += line + '\n';
    }

    return content;
}

void Initialize(Platform::Window* window)
{
    // Disable depth testing for UI rendering so depth values do not affect
    // draw order. UI elements are rendered strictly in the order of glDraw calls.
    // glEnable(GL_DEPTH_TEST);

    std::string fontShaderVert = ReadShaderFile("resources/shaders/font.vert.glsl");
    std::string fontShaderFrag = ReadShaderFile("resources/shaders/font.frag.glsl");

    g_window = window;

    shaderProgram         = glCreateProgram();
    GLuint vertexShader   = CompileShader(fontShaderVert.c_str(), GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(fontShaderFrag.c_str(), GL_FRAGMENT_SHADER);

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
    Platform::Size fbSize = GetFramebufferSize(g_window);
    glViewport(0, 0, fbSize.width, fbSize.height);

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void EndFrame()
{
    Platform::SwapBuffers(g_window);
}

struct Size
{
    float width;
    float height;
};

struct Color
{
    float r;
    float g;
    float b;
    float a;
};

struct UVMap
{
    Math::Vec2 bottomLeft  = { 0.0f, 0.0f };
    Math::Vec2 topLeft     = { 0.0f, 1.0f };
    Math::Vec2 topRight    = { 1.0f, 1.0f };
    Math::Vec2 bottomRight = { 1.0f, 0.0f };
};

typedef uint32_t Texture;

Texture CreateTexture(int width, int height, int componenets, const void* data)
{
    Texture texture {};

    GLenum internalFormat;
    GLenum pixelFormat;

    switch (componenets)
    {
        case 1:
        {
            internalFormat = GL_R8;
            pixelFormat    = GL_RED;
            break;
        }
        case 3:
        {
            internalFormat = GL_RGB8;
            pixelFormat    = GL_RGB;
            break;
        }
        case 4:
        {
            internalFormat = GL_RGBA8;
            pixelFormat    = GL_RGBA;
            break;
        }
        default:
        {
            std::cout << "Unsupported texture format.\n";

            return texture;
        }
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(
      GL_TEXTURE_2D, 0, internalFormat, width, height, 0, pixelFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return texture;
}

void BindTexture(Texture texture, int pos = 0)
{
    glActiveTexture(GL_TEXTURE0 + pos);
    glBindTexture(GL_TEXTURE_2D, texture);

    /// TODO: this should be called once on renderer initialization probably.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void DrawQuad(Math::Vec2 pos, Size size, Color color, UVMap uvMap = {})
{
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    float normX = Math::Map(pos.x, (float)viewport[0], (float)viewport[2], -1.0f, 1.0f);
    float normY = Math::Map(pos.y, (float)viewport[1], (float)viewport[3], -1.0f, 1.0f);
    float normW =
      Math::Map(pos.x + size.width, (float)viewport[0], (float)viewport[2], -1.0f, 1.0f);
    float normH =
      Math::Map(pos.y + size.height, (float)viewport[1], (float)viewport[3], -1.0f, 1.0f);

    float vertices[] {
        normX, -normH, uvMap.bottomLeft.x,  uvMap.bottomLeft.y,  // bottom left
        normX, -normY, uvMap.topLeft.x,     uvMap.topLeft.y,     // top left
        normW, -normY, uvMap.topRight.x,    uvMap.topRight.y,    // top right
        normW, -normH, uvMap.bottomRight.x, uvMap.bottomRight.y, // bottom right
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

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shaderProgram);

    float r = Math::Map(color.r, 0.0f, 255.0f, 0.0f, 1.0f);
    float g = Math::Map(color.g, 0.0f, 255.0f, 0.0f, 1.0f);
    float b = Math::Map(color.b, 0.0f, 255.0f, 0.0f, 1.0f);
    float a = Math::Map(color.a, 0.0f, 255.0f, 0.0f, 1.0f);

    glUniform4f(glGetUniformLocation(shaderProgram, "uColor"), r, g, b, a);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDeleteBuffers(vbo, &vbo);
    glDeleteBuffers(ebo, &ebo);
    glDeleteVertexArrays(vao, &vao);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // namespace DF::Render
