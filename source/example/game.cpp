#include <cstdio>

#include "game.hpp"

#include "defacto_api.hpp"

#pragma warning(push, 0)
#define CLAY_IMPLEMENTATION
#include "clay.h"
#pragma warning(pop)

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
    DF::Assets::Font* font = (DF::Assets::Font*)userData;

    float width  = 0;
    float height = font->lineHeight * config->fontSize;

    for (int pos = 0; pos < text.length; ++pos)
    {
        for (int i = 0; i < font->glyphCount; ++i)
        {
            if (text.chars[pos] == font->glyphs[i].code)
            {
                width += font->glyphs[i].advance * config->fontSize;

                break;
            }
        }
    }

    return Clay_Dimensions { width, height };
}

void RenderHeaderButton(Clay_String text)
{
    CLAY_AUTO_ID({ .layout          = { .padding = { 16, 16, 8, 8 } },
                   .backgroundColor = { 140, 140, 140, 255 },
                   .cornerRadius    = CLAY_CORNER_RADIUS(5) })
    {
        CLAY_TEXT(text,
                  CLAY_TEXT_CONFIG({
                    .textColor = { 255, 255, 255, 255 },
                    .fontId    = 0,
                    .fontSize  = 16,
                  }));
    }
}

Clay_RenderCommandArray CreateUI()
{
    Clay_BeginLayout();

    Clay_Sizing layoutExpand = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) };

    Clay_Color contentBackgroundColor = { 90, 90, 90, 255 };

    //RenderHeaderButton(CLAY_STRING("Upload"));

    CLAY(CLAY_ID("ScrollingContainer"),
         {
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(0),
                    .height = CLAY_SIZING_FIXED(0),
                },
                .padding = { 16, 16, 16, 16 },
                .childGap = 16,
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
            },
           .backgroundColor = { 200, 200, 100, 255 },
           .cornerRadius    = CLAY_CORNER_RADIUS(10),
           .clip            = { .vertical = true, .childOffset = Clay_GetScrollOffset() },
         })
    {
        for (int i = 0; i < 100; ++i)
        {
            CLAY_TEXT(CLAY_STRING("Test string"),
                      CLAY_TEXT_CONFIG({
                        .textColor = { 255, 255, 255, 255 },
                        .fontId    = 0,
                        .fontSize  = 16,
                      }));
        }
    }

    return Clay_EndLayout();
}

DF_EXPORT_C GAME_RELOAD(GameReload)
{
    uint64_t clayMemorySize = Clay_MinMemorySize();
    uint64_t fontMemorySize = sizeof(DF::Assets::Font) + sizeof(DF::Assets::Font::Glyph) * 95;

    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(
      clayMemorySize, (char*)gameMemory.permanentStorage + fontMemorySize);
    Clay_Initialize(
      clayMemory, Clay_Dimensions { 800, 600 }, Clay_ErrorHandler { HandleClayErrors });

    Clay_SetMeasureTextFunction(MeasureText, gameMemory.permanentStorage);
}

DF_EXPORT_C GAME_INITIALIZE(GameInitialize)
{
    uint64_t clayMemorySize = Clay_MinMemorySize();
    uint64_t fontMemorySize = sizeof(DF::Assets::Font) + sizeof(DF::Assets::Font::Glyph) * 95;
    DF::GameMemory gameMemory {};

    gameMemory.permanentStorageSize = clayMemorySize + fontMemorySize;
    gameMemory.permanentStorage     = DF::Platform::AllocateMemory(gameMemory.permanentStorageSize);

    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(
      clayMemorySize, (char*)gameMemory.permanentStorage + fontMemorySize);
    Clay_Initialize(
      clayMemory, Clay_Dimensions { 800, 600 }, Clay_ErrorHandler { HandleClayErrors });

    bool roboto = DF::Assets::LoadFont("resources/fonts/roboto_regular.json",
                                       "resources/fonts/roboto_regular.png",
                                       (DF::Assets::Font*)gameMemory.permanentStorage);
    if (roboto)
    {
        Clay_SetMeasureTextFunction(MeasureText, gameMemory.permanentStorage);
    }

    return gameMemory;
}

struct InputState
{
    DF::Math::Vec2 scrollPos;
    DF::Math::Vec2 mousePos;
};

DF_EXPORT_C GAME_UPDATE(GameUpdate)
{
    float scrollSpeed = 4;
    DF::Math::Vec2 scroll = { 0.0f, 0.0f };
    // DF::Math::Vec2 scrollDelta =
    //   DF::Math::Vec2 { scroll.x - g_lastScroll.x, scroll.y - g_lastScroll.y };
    // g_lastScroll           = scroll;

    DF::Math::Vec2 mousePos = DF::Platform::GetCursorPos();
    bool lmbPressed         = DF::Platform::MouseKeyPressed(DF::MouseKey::LEFT);

    Clay_SetPointerState(Clay_Vector2 { mousePos.x, mousePos.y }, lmbPressed);

    // Clay_UpdateScrollContainers needs to be called before Clay_BeginLayout for the position
    // to avoid a 1 frame delay
    Clay_UpdateScrollContainers(
      true, // Enable drag scrolling
      Clay_Vector2 {
        scroll.x * scrollSpeed,
        scroll.y * scrollSpeed,
      }, // Clay_Vector2 scrollwheel / trackpad scroll x and y delta this frame
      dt // Time since last frame in seconds as a float e.g. 8ms is 0.008f
    );

    Clay_RenderCommandArray renderCommands = CreateUI();

    DF::Size fbSize = DF::Platform::GetFramebufferSize();

    Clay_SetLayoutDimensions(Clay_Dimensions { (float)fbSize.width, (float)fbSize.height });

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

                DF::Render::DrawText((DF::Assets::Font*)gameMemory.permanentStorage,
                                     text.stringContents.chars,
                                     text.fontSize,
                                     { boundingBox.x, boundingBox.y },
                                     text.stringContents.length);

                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE:
            {
                Clay_RectangleRenderData rrd = renderCommand->renderData.rectangle;

                DF::Render::DrawQuad({ boundingBox.x, boundingBox.y },
                                     { (int)boundingBox.width, (int)boundingBox.height },
                                     { rrd.backgroundColor.r,
                                       rrd.backgroundColor.g,
                                       rrd.backgroundColor.b,
                                       rrd.backgroundColor.a });

                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START:
            {
                DF::Render::BeginScissor((int)boundingBox.x,
                                         (int)boundingBox.y,
                                         (int)boundingBox.width,
                                         (int)boundingBox.height);
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END:
            {
                DF::Render::EndScissor();
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_BORDER:
            {
                // The renderer should draw a colored border inset into the bounding box.
                Clay_BorderRenderData brd = renderCommand->renderData.border;

                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_IMAGE:
            {
                break;
            }
            default:
            {
                // printf("Unhandled render command %d\r\n", renderCommand->commandType);
                break;
            }
        }
    }
}
