#include <fstream>
#include <iostream>

#pragma warning(push, 0)
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <json.hpp>
#include <gl/glew.h>
#pragma warning(pop)

namespace DF::Assets
{

typedef uint32_t Texture;

struct Font
{
    Texture bitmap;
    float size;
    float atlasWidth;
    float atlasHeight;
    float lineHeight;
    bool valid;

    struct Glyph
    {
        int code;
        float advance;

        struct AtlasBounds
        {
            float top;
            float right;
            float bottom;
            float left;
        };
        AtlasBounds atlasBounds;

        struct PlaneBounds
        {
            float top;
            float right;
            float bottom;
            float left;
        };
        PlaneBounds planeBounds;
    };
    std::vector<Glyph> glyphs;
};

static Texture CreateTexture(int width, int height, int componenets, const void* data)
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

static Font LoadFont(const char* pathJson, const char* pathBitmap)
{
    Font font {};

    std::ifstream file(pathJson);

    if (!file.is_open())
    {
        std::cout << "Failed to open font file: " << pathJson << '\n';

        return font;
    }

    using json = nlohmann::json;

    json data = json::parse(file);

    font.size        = data["atlas"]["size"];
    font.atlasWidth  = data["atlas"]["width"];
    font.atlasHeight = data["atlas"]["height"];
    font.lineHeight  = data["metrics"]["lineHeight"];

    for (const auto& glyph : data["glyphs"])
    {
        Font::Glyph::AtlasBounds atlasBounds {};
        Font::Glyph::PlaneBounds planeBounds {};

        if (glyph.contains("atlasBounds"))
        {
            atlasBounds.top =
              Math::Map(glyph["atlasBounds"]["top"], 0, font.atlasHeight, 0.0f, 1.0f);
            atlasBounds.bottom =
              Math::Map(glyph["atlasBounds"]["bottom"], 0, font.atlasHeight, 0.0f, 1.0f);
            atlasBounds.left =
              Math::Map(glyph["atlasBounds"]["left"], 0, font.atlasWidth, 0.0f, 1.0f);
            atlasBounds.right =
              Math::Map(glyph["atlasBounds"]["right"], 0, font.atlasWidth, 0.0f, 1.0f);
        }

        if (glyph.contains("planeBounds"))
        {
            planeBounds.top    = glyph["planeBounds"]["top"];
            planeBounds.bottom = glyph["planeBounds"]["bottom"];
            planeBounds.left   = glyph["planeBounds"]["left"];
            planeBounds.right  = glyph["planeBounds"]["right"];
        }

        font.glyphs.push_back({
          glyph["unicode"],
          glyph["advance"],
          atlasBounds,
          planeBounds,
        });
    }

    stbi_set_flip_vertically_on_load(true);

    int width, height, components;
    uint8_t* bitmap = stbi_load(pathBitmap, &width, &height, &components, 0);

    if (!bitmap)
    {
        std::cout << "Failed to load font bitmap: " << stbi_failure_reason() << '\n';

        return font;
    }

    font.bitmap = CreateTexture(width, height, components, bitmap);
    stbi_image_free(bitmap);

    return font;
}

} // namespace DF::Assets
