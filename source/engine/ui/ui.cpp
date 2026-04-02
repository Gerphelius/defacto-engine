#include <cstdio>
#include <iostream>
#include <vector>

#pragma warning(push, 0)
#define CLAY_IMPLEMENTATION
#include "clay.h"

#include "template.cpp"
#pragma warning(pop)

namespace DF::UI
{
ClayVideoDemo_Data demo_data;
Clay_RenderCommandArray renderCommands;

const Clay_Color COLOR_LIGHT  = Clay_Color { 224, 215, 210, 255 };
const Clay_Color COLOR_RED    = Clay_Color { 168, 66, 28, 255 };
const Clay_Color COLOR_ORANGE = Clay_Color { 225, 138, 50, 255 };

static void HandleClayErrors(Clay_ErrorData errorData)
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

static inline Clay_Dimensions MeasureText(Clay_StringSlice text,
                                          Clay_TextElementConfig* config,
                                          void* userData)
{
    Assets::Font* font = (Assets::Font*)userData;

    float width  = 0;
    float height = font->lineHeight * config->fontSize;

    for (int pos = 0; pos < text.length; ++pos)
    {
        for (Assets::Font::Glyph glyph : font->glyphs)
        {
            if (text.chars[pos] == glyph.code)
            {
                width += glyph.advance * config->fontSize;

                break;
            }
        }
    }

    return Clay_Dimensions { width, height };
}

Assets::Font g_baseFont;

static void Initialize()
{
    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemory =
      Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));

    Clay_Initialize(
      clayMemory, Clay_Dimensions { 800, 600 }, Clay_ErrorHandler { HandleClayErrors });

    demo_data = ClayVideoDemo_Initialize();
    // renderCommands = GetTestLayout();

    g_baseFont =
      Assets::LoadFont("resources/fonts/roboto_regular.json", "resources/fonts/roboto_regular.png");
    Clay_SetMeasureTextFunction(MeasureText, &g_baseFont);
}

static void Render(int width, int height, Math::Vec2 mousePos, bool lmbPressed)
{
    Clay_SetPointerState(Clay_Vector2 { mousePos.x, mousePos.y }, lmbPressed);

    // Clay_UpdateScrollContainers needs to be called before Clay_BeginLayout for the position to
    // avoid a 1 frame delay
    //Clay_UpdateScrollContainers(
    //  true,            // Enable drag scrolling
    //  scrollDelta,     // Clay_Vector2 scrollwheel / trackpad scroll x and y delta this frame
    //  float deltaTime, // Time since last frame in seconds as a float e.g. 8ms is 0.008f
    //);

     renderCommands = ClayVideoDemo_CreateLayout(&demo_data);

     Clay_SetLayoutDimensions(Clay_Dimensions { (float)width, (float)height });

     for (int j = 0; j < renderCommands.length; j++)
    {
         Clay_RenderCommand* renderCommand = Clay_RenderCommandArray_Get(&renderCommands, j);
         Clay_BoundingBox boundingBox      = renderCommand->boundingBox;

        switch (renderCommand->commandType)
        {
            case CLAY_RENDER_COMMAND_TYPE_TEXT:
            {
                Clay_RenderCommand* rc = renderCommand;

                Clay_Color c             = renderCommand->renderData.text.textColor;
                uint16_t font_id         = renderCommand->renderData.text.fontId;
                Clay_TextRenderData text = rc->renderData.text;

                Render::DrawText(g_baseFont,
                                 text.stringContents.chars,
                                 text.fontSize,
                                 { boundingBox.x, boundingBox.y },
                                 text.stringContents.length);

                // Render::DrawQuad({ boundingBox.x, boundingBox.y },
                //                  { boundingBox.width, boundingBox.height },
                //                  { c.r, c.g, c.b, c.a });

                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE:
            {
                Clay_RectangleRenderData rrd = renderCommand->renderData.rectangle;

                Render::DrawQuad({ boundingBox.x, boundingBox.y },
                                 { boundingBox.width, boundingBox.height },
                                 { rrd.backgroundColor.r,
                                   rrd.backgroundColor.g,
                                   rrd.backgroundColor.b,
                                   rrd.backgroundColor.a });

                break;
            }

            // The renderer should draw a colored border inset into the bounding box.
            case CLAY_RENDER_COMMAND_TYPE_BORDER:
            {
                Clay_BorderRenderData brd = renderCommand->renderData.border;

                break;
            }

            case CLAY_RENDER_COMMAND_TYPE_IMAGE:
            {
                break;
            }

            default:
                // printf("Unhandled render command %d\r\n", renderCommand->commandType);
                break;
        }
    }
}

} // namespace DF::UI
