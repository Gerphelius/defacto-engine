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

#include "defacto_api.hpp"

namespace DF::Assets
{

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

DF_API Font* LoadFont(const char* pathJson, const char* pathBitmap, Arena* arena)
{
    std::ifstream file(pathJson);

    if (!file.is_open())
    {
        std::cout << "Failed to open font file: " << pathJson << '\n';

        return nullptr;
    }

    stbi_set_flip_vertically_on_load(true);

    int width, height, components;
    uint8_t* bitmap = stbi_load(pathBitmap, &width, &height, &components, 0);

    if (!bitmap)
    {
        std::cout << "Failed to load font bitmap: " << stbi_failure_reason() << '\n';

        return nullptr;
    }

    using json = nlohmann::json;
    json data  = json::parse(file);

    int glyphsCount = (int)data["glyphs"].size();
    int fontSize    = sizeof(Font) + sizeof(Font::Glyph) * glyphsCount;
    Font* font      = (Font*)ArenaPush(arena, fontSize);

    font->bitmap = CreateTexture(width, height, components, bitmap);
    stbi_image_free(bitmap);

    font->size          = data["atlas"]["size"];
    font->atlasWidth    = data["atlas"]["width"];
    font->atlasHeight   = data["atlas"]["height"];
    font->distanceRange = data["atlas"]["distanceRange"];
    font->lineHeight    = data["metrics"]["lineHeight"];
    font->glyphCount    = glyphsCount;
    font->glyphs        = (Font::Glyph*)(font + 1);

    json::array_t glyphs = data["glyphs"];

    for (int i = 0; i < font->glyphCount; ++i)
    {
        Font::Glyph* glyph = font->glyphs + i;

        glyph->code    = glyphs[i]["unicode"];
        glyph->advance = glyphs[i]["advance"];

        if (glyphs[i].contains("atlasBounds"))
        {
            glyph->atlasBounds.top =
              Math::Map(glyphs[i]["atlasBounds"]["top"], 0, font->atlasHeight, 0.0f, 1.0f);
            glyph->atlasBounds.bottom =
              Math::Map(glyphs[i]["atlasBounds"]["bottom"], 0, font->atlasHeight, 0.0f, 1.0f);
            glyph->atlasBounds.left =
              Math::Map(glyphs[i]["atlasBounds"]["left"], 0, font->atlasWidth, 0.0f, 1.0f);
            glyph->atlasBounds.right =
              Math::Map(glyphs[i]["atlasBounds"]["right"], 0, font->atlasWidth, 0.0f, 1.0f);
        }

        if (glyphs[i].contains("planeBounds"))
        {
            glyph->planeBounds.top    = glyphs[i]["planeBounds"]["top"];
            glyph->planeBounds.bottom = glyphs[i]["planeBounds"]["bottom"];
            glyph->planeBounds.left   = glyphs[i]["planeBounds"]["left"];
            glyph->planeBounds.right  = glyphs[i]["planeBounds"]["right"];
        }
    }

    return font;
}

} // namespace DF::Assets
