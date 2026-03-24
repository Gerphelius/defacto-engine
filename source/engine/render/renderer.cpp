#include <cstdint>
#include <gl/glew.h>
#include <iostream>

#include "platform/platform_window.hpp"

namespace DF::RENDER
{
const char* vert = R"(
    #version 330 core

    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec2 aTexCoord;

    out vec2 TexCoord;

    void main()
    {
        gl_Position = vec4(aPos, 1.0f);
        TexCoord = aTexCoord;
    }
)";

const char* frag = R"(
    #version 330 core

    in vec2 TexCoord;

    out vec4 FragColor;

    uniform vec4 uColor;
    uniform sampler2D uTexture;

    void main()
    {
        /// TODO: review smoothing calculation. Should be based on
        /// atlas.distanceRange and relative font scale

        float smoothing = 0.5;
        float dist = texture2D(uTexture, TexCoord).r;
        float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, dist);
        FragColor = vec4(uColor.rgb, uColor.a * alpha);

        // FragColor = vec4(uColor.r, uColor.g, uColor.b, texture(uTexture, TexCoord).r);
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

struct Vec2
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

struct UVMap
{
    Vec2 bottomLeft  = { 0.0f, 0.0f };
    Vec2 topLeft     = { 0.0f, 1.0f };
    Vec2 topRight    = { 1.0f, 1.0f };
    Vec2 bottomRight = { 1.0f, 0.0f };
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

void DrawQuad(Vec2 pos, Size size, Color color, UVMap uvMap = {})
{
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    float normX = Map(pos.x, (float)viewport[0], (float)viewport[2], -1.0f, 1.0f);
    float normY = Map(pos.y, (float)viewport[1], (float)viewport[3], -1.0f, 1.0f);
    float normW = Map(pos.x + size.width, (float)viewport[0], (float)viewport[2], -1.0f, 1.0f);
    float normH = Map(pos.y + size.height, (float)viewport[1], (float)viewport[3], -1.0f, 1.0f);

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
