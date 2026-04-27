#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <gl/glew.h>

namespace DF::Render
{

static GLuint CompileShader(const char* source, GLenum type)
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

static std::string ReadShaderFile(const std::string& path)
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

typedef uint32_t Shader;

static Shader CreateShader(const char* vertSrc, const char* fragSrc)
{
    Shader shader         = glCreateProgram();
    GLuint vertexShader   = CompileShader(vertSrc, GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(fragSrc, GL_FRAGMENT_SHADER);

    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);

    int success;
    char infoLog[512];
    glGetProgramiv(shader, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shader, 512, NULL, infoLog);

        std::cout << "Shader program error: " << infoLog << '\n';
    }

    // Need to detach shader before deletion because the driver
    // won't free GPU memory until the shader program is deleted
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDeleteShader.xhtml
    glDetachShader(shader, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(shader, fragmentShader);
    glDeleteShader(fragmentShader);

    return shader;
}

static Shader g_shapeShader;
static Shader g_fontShader;
static Window* g_window;

DF_API void Initialize(Window* window)
{
    // Disable depth testing for UI rendering so depth values do not affect
    // draw order. UI elements are rendered strictly in the order of glDraw calls.
    // glEnable(GL_DEPTH_TEST);

    std::string fontShaderVert = ReadShaderFile("resources/shaders/font.vert.glsl");
    std::string fontShaderFrag = ReadShaderFile("resources/shaders/font.frag.glsl");
    g_fontShader               = CreateShader(fontShaderVert.c_str(), fontShaderFrag.c_str());

    std::string shapeShaderVert = ReadShaderFile("resources/shaders/shape.vert.glsl");
    std::string shapeShaderFrag = ReadShaderFile("resources/shaders/shape.frag.glsl");
    g_shapeShader               = CreateShader(shapeShaderVert.c_str(), shapeShaderFrag.c_str());

    g_window = window;

    /// TODO: this should be called once on renderer initialization probably.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // CreateShader(const char* vertSrc, const char* fragSrc);
}

DF_API void BeginFrame()
{
    Size fbSize = Platform::GetFramebufferSize();
    glViewport(0, 0, fbSize.width, fbSize.height);

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

DF_API void EndFrame()
{
    Platform::SwapBuffers(g_window);
}

// Goes top to bottom, left to right.
DF_API void BeginScissor(int x, int y, int width, int height)
{
    Size fbSize = Platform::GetFramebufferSize();

    glEnable(GL_SCISSOR_TEST);
    glScissor(x, fbSize.height - (y + height), width, height);
}

DF_API void EndScissor()
{
    glDisable(GL_SCISSOR_TEST);
}

void BindTexture(Texture texture, int pos = 0)
{
    glActiveTexture(GL_TEXTURE0 + pos);
    glBindTexture(GL_TEXTURE_2D, texture);
}

// TODO: Need to rework this to gather vertices in some buffer so it can be called outside
// Begin/EndFrame boundaries
DF_API void DrawQuad(Vec2 pos, Size size, Color color, UVMap uvMap)
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

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(g_shapeShader);

    float r = Map(color.r, 0.0f, 255.0f, 0.0f, 1.0f);
    float g = Map(color.g, 0.0f, 255.0f, 0.0f, 1.0f);
    float b = Map(color.b, 0.0f, 255.0f, 0.0f, 1.0f);
    float a = Map(color.a, 0.0f, 255.0f, 0.0f, 1.0f);

    glUniform4f(glGetUniformLocation(g_shapeShader, "uColor"), r, g, b, a);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDeleteBuffers(vbo, &vbo);
    glDeleteBuffers(ebo, &ebo);
    glDeleteVertexArrays(vao, &vao);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// TODO: Need to rework this to gather vertices in some buffer so it can be called outside
// Begin/EndFrame boundaries
DF_API void DrawText(const Font* font, const char* str, int fontSize, Vec2 pos, int strlen)
{
    // TODO: Accept DF::String and get rid of vectors

    std::vector<float> vertices {};
    std::vector<int> indices {};

    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    int offset = 0;

    for (int i = 0; strlen ? i < strlen : str[i]; ++i)
    {
        for (int glyphNum = 0; glyphNum < font->glyphCount; ++glyphNum)
        {
            Font::Glyph glyph = font->glyphs[glyphNum];

            if (glyph.code == str[i])
            {
                float width   = (glyph.planeBounds.right - glyph.planeBounds.left) * fontSize;
                float height  = (glyph.planeBounds.top - glyph.planeBounds.bottom) * fontSize;
                float offsetY = (pos.y - height) - (glyph.planeBounds.bottom * fontSize) + fontSize;

                if (width && height)
                {
                    Vec2 bottomL = { glyph.atlasBounds.left, glyph.atlasBounds.bottom };
                    Vec2 topL    = { glyph.atlasBounds.left, glyph.atlasBounds.top };
                    Vec2 topR    = { glyph.atlasBounds.right, glyph.atlasBounds.top };
                    Vec2 bottomR = { glyph.atlasBounds.right, glyph.atlasBounds.bottom };

                    float normX = Map(pos.x, (float)viewport[0], (float)viewport[2], -1.0f, 1.0f);
                    float normY = Map(offsetY, (float)viewport[1], (float)viewport[3], -1.0f, 1.0f);
                    float normW =
                      Map(pos.x + width, (float)viewport[0], (float)viewport[2], -1.0f, 1.0f);
                    float normH =
                      Map(offsetY + height, (float)viewport[1], (float)viewport[3], -1.0f, 1.0f);

                    vertices.insert(vertices.end(),
                                    {
                                      normX,
                                      -normH,
                                      bottomL.x,
                                      bottomL.y, // bottom left
                                      normX,
                                      -normY,
                                      topL.x,
                                      topL.y, // top left
                                      normW,
                                      -normY,
                                      topR.x,
                                      topR.y, // top right
                                      normW,
                                      -normH,
                                      bottomR.x,
                                      bottomR.y, // bottom right
                                    });
                    indices.insert(indices.end(),
                                   {
                                     0 + 4 * offset,
                                     1 + 4 * offset,
                                     3 + 4 * offset,
                                     1 + 4 * offset,
                                     2 + 4 * offset,
                                     3 + 4 * offset,
                                   });
                    ++offset;
                }

                pos.x += (glyph.advance * fontSize);

                break;
            }
        }
    }

    unsigned int vbo;
    glGenBuffers(1, &vbo);

    unsigned int ebo;
    glGenBuffers(1, &ebo);

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
      GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindTexture(GL_TEXTURE_2D, font->bitmap);
    glUseProgram(g_fontShader);

    float smoothing = (1.0f / font->distanceRange) / (fontSize / font->size);

    glUniform4f(glGetUniformLocation(g_fontShader, "uColor"), 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(g_fontShader, "uSmoothing"), smoothing);

    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

    glDeleteBuffers(vbo, &vbo);
    glDeleteBuffers(ebo, &ebo);
    glDeleteVertexArrays(vao, &vao);
}

} // namespace DF::Render
