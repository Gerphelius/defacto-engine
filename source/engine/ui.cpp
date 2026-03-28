#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

#pragma warning(push, 0)
#define CLAY_IMPLEMENTATION
#include "clay.h"

#include "template.cpp"
#pragma warning(pop)

#include <json.hpp>

namespace DF::UI
{
ClayVideoDemo_Data demo_data;
Clay_RenderCommandArray renderCommands;

const Clay_Color COLOR_LIGHT  = Clay_Color { 224, 215, 210, 255 };
const Clay_Color COLOR_RED    = Clay_Color { 168, 66, 28, 255 };
const Clay_Color COLOR_ORANGE = Clay_Color { 225, 138, 50, 255 };

void HandleClayErrors(Clay_ErrorData errorData)
{
    // See the Clay_ErrorData struct for more information
    printf("%s", errorData.errorText.chars);
    switch (errorData.errorType)
    {
        // etc
        case CLAY_ERROR_TYPE_TEXT_MEASUREMENT_FUNCTION_NOT_PROVIDED:
        {
            break;
        }
        case CLAY_ERROR_TYPE_ARENA_CAPACITY_EXCEEDED:
        case CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED:
        case CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED:
        case CLAY_ERROR_TYPE_DUPLICATE_ID:
        case CLAY_ERROR_TYPE_FLOATING_CONTAINER_PARENT_NOT_FOUND:
        case CLAY_ERROR_TYPE_PERCENTAGE_OVER_1:
        case CLAY_ERROR_TYPE_INTERNAL_ERROR:
        case CLAY_ERROR_TYPE_UNBALANCED_OPEN_CLOSE:
            break;
    }
}

struct FontData
{
    const char* name;
    float size;
    float atlasWidth;
    float atlasHeight;

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

FontData GetFontData()
{
    FontData fontData {};

    const char* font = "resources/fonts/bitcount_regular.json";
    std::ifstream file(font);

    if (!file.is_open())
    {
        std::cout << "Failed to open font file: " << font << '\n';

        return fontData;
    }

    using json = nlohmann::json;

    json data = json::parse(file);

    fontData.name        = "bitcount";
    fontData.size        = data["atlas"]["size"];
    fontData.atlasWidth  = data["atlas"]["width"];
    fontData.atlasHeight = data["atlas"]["height"];

    for (const auto& glyph : data["glyphs"])
    {
        FontData::Glyph::AtlasBounds atlasBounds {};
        FontData::Glyph::PlaneBounds planeBounds {};

        if (glyph.contains("atlasBounds"))
        {
            atlasBounds.top    = glyph["atlasBounds"]["top"];
            atlasBounds.bottom = glyph["atlasBounds"]["bottom"];
            atlasBounds.left   = glyph["atlasBounds"]["left"];
            atlasBounds.right  = glyph["atlasBounds"]["right"];
        }

        if (glyph.contains("planeBounds"))
        {
            planeBounds.top    = glyph["planeBounds"]["top"];
            planeBounds.bottom = glyph["planeBounds"]["bottom"];
            planeBounds.left   = glyph["planeBounds"]["left"];
            planeBounds.right  = glyph["planeBounds"]["right"];
        }

        fontData.glyphs.push_back({
          glyph["unicode"],
          glyph["advance"],
          atlasBounds,
          planeBounds,
        });
    }

    return fontData;
}

FontData g_fontData;

void RenderText(const char* str, int fontSize, float x, float y, int strlen = -1)
{
    if (strlen <= -1)
    {
        for (const char* ch = str; *ch; *ch++)
        {
            for (const auto& glyph : g_fontData.glyphs)
            {
                if (glyph.code == *ch)
                {
                    float width   = (glyph.planeBounds.right - glyph.planeBounds.left) * fontSize;
                    float height  = (glyph.planeBounds.top - glyph.planeBounds.bottom) * fontSize;
                    float offsetY = (y - height) - (glyph.planeBounds.bottom * fontSize);

                    if (width && height)
                    {
                        float topNorm =
                          Math::Map(glyph.atlasBounds.top, 0, g_fontData.atlasHeight, 0.0f, 1.0f);
                        float rightNorm =
                          Math::Map(glyph.atlasBounds.right, 0, g_fontData.atlasWidth, 0.0f, 1.0f);
                        float bottomNorm = Math::Map(
                          glyph.atlasBounds.bottom, 0, g_fontData.atlasHeight, 0.0f, 1.0f);
                        float leftNorm =
                          Math::Map(glyph.atlasBounds.left, 0, g_fontData.atlasWidth, 0.0f, 1.0f);

                        Math::Vec2 bottomL = { leftNorm, bottomNorm };
                        Math::Vec2 topL    = { leftNorm, topNorm };
                        Math::Vec2 topR    = { rightNorm, topNorm };
                        Math::Vec2 bottomR = { rightNorm, bottomNorm };

                        Render::DrawQuad({ x, offsetY },
                                         { width, height },
                                         { 255.0, 255.0, 255.0, 255.0 },
                                         { bottomL, topL, topR, bottomR });
                    }

                    x += (glyph.advance * fontSize);

                    break;
                }
            }
        }
    }
}

static inline Clay_Dimensions MeasureText(Clay_StringSlice text,
                                          Clay_TextElementConfig* config,
                                          void* userData)
{
    // Clay_TextElementConfig contains members such as fontId, fontSize, letterSpacing etc
    // Note: Clay_String->chars is not guaranteed to be null terminated
    return Clay_Dimensions {
        .width =
          (float)(text.length *
                  config->fontSize), // <- this will only work for monospace fonts, see the
                                     // renderers/ directory for more advanced text measurement
        .height = (float)config->fontSize,
    };
}

/// TODO: remove this
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Initialize()
{
    g_fontData = GetFontData();

    stbi_set_flip_vertically_on_load(true);

    int width, height, components;
    unsigned char* data =
      stbi_load("resources/fonts/bitcount_regular.png", &width, &height, &components, 0);

    if (!data)
    {
        std::cout << "Failed to load image: " << stbi_failure_reason() << '\n';
    }
    else
    {
        Render::Texture texture = Render::CreateTexture(width, height, components, data);
        Render::BindTexture(texture);

        stbi_image_free(data);
    }

    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemory =
      Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));

    Clay_Initialize(
      clayMemory, Clay_Dimensions { 800, 600 }, Clay_ErrorHandler { HandleClayErrors });

    demo_data = ClayVideoDemo_Initialize();
    // renderCommands = GetTestLayout();

    int bb = 0;

    Clay_SetMeasureTextFunction(MeasureText, &bb);
}

void Render(int width, int height)
{
    RenderText("This is some example text.", 64, 100, 100);

    // renderCommands = ClayVideoDemo_CreateLayout(&demo_data);

    // Clay_SetLayoutDimensions(Clay_Dimensions { (float)width, (float)height });

    // for (int j = 0; j < renderCommands.length; j++)
    // {
    //     Clay_RenderCommand* renderCommand = Clay_RenderCommandArray_Get(&renderCommands, j);
    //     Clay_BoundingBox boundingBox      = renderCommand->boundingBox;

    //     switch (renderCommand->commandType)
    //     {
    //         case CLAY_RENDER_COMMAND_TYPE_TEXT:
    //         {
    //             Clay_RenderCommand* rc = renderCommand;

    //             Clay_Color c     = renderCommand->renderData.text.textColor;
    //             uint16_t font_id = renderCommand->renderData.text.fontId;

    //             RENDER::DrawQuad({ boundingBox.x, boundingBox.y },
    //                                  { boundingBox.width, boundingBox.height },
    //                                  { c.r, c.g, c.b, c.a });

    //             break;
    //         }
    //         case CLAY_RENDER_COMMAND_TYPE_RECTANGLE:
    //         {
    //             Clay_RectangleRenderData rrd = renderCommand->renderData.rectangle;

    //             RENDER::DrawQuad({ boundingBox.x, boundingBox.y },
    //                                  { boundingBox.width, boundingBox.height },
    //                                  { rrd.backgroundColor.r,
    //                                    rrd.backgroundColor.g,
    //                                    rrd.backgroundColor.b,
    //                                    rrd.backgroundColor.a });

    //             break;
    //         }

    //         // The renderer should draw a colored border inset into the bounding box.
    //         case CLAY_RENDER_COMMAND_TYPE_BORDER:
    //         {
    //             Clay_BorderRenderData brd = renderCommand->renderData.border;

    //             break;
    //         }

    //         case CLAY_RENDER_COMMAND_TYPE_IMAGE:
    //         {
    //             break;
    //         }

    //         default:
    //             // printf("Unhandled render command %d\r\n", renderCommand->commandType);
    //             break;
    //     }
    // }
}
} // namespace DF::UI
