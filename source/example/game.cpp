#include <cstdio>

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

void Button(Clay_String text)
{
    CLAY_AUTO_ID({ .layout          = { .padding = { 60, 60, 8, 8 } },
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

    CLAY(CLAY_ID("OuterContainer"),
        {
            .layout          = {
                .sizing          = { .width = CLAY_SIZING_FIXED(0), .height = CLAY_SIZING_GROW(0) },
                .padding         = CLAY_PADDING_ALL(16),
                .childGap        = 16,
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
            },
            .backgroundColor = { 43, 41, 51, 255 },
            .clip            = { .vertical = true, .childOffset = Clay_GetScrollOffset() },
        })
    {
        for (int i = 0; i < 100; ++i)
        {
            Button(CLAY_STRING("Test"));
        }
    }

    return Clay_EndLayout();
}

enum class Fonts
{
    ROBOTO,

    TOTAL,
};

struct GameState
{
    DF::Arena fontsArena;
    DF::Assets::Font* fonts[(int)Fonts::TOTAL];

    DF::Arena clayArena;

    // DF::Arena entities;
    // ...
};

DF_EXPORT_C GAME_RELOAD(GameReload)
{
    GameState* gameState = (GameState*)gameMemory.permanent.base;
    Clay_Arena clayMemory =
      Clay_CreateArenaWithCapacityAndMemory(gameState->clayArena.size, gameState->clayArena.base);
    Clay_Initialize(
      clayMemory, Clay_Dimensions { 800, 600 }, Clay_ErrorHandler { HandleClayErrors });
    Clay_SetMeasureTextFunction(MeasureText, gameState->fonts[(int)Fonts::ROBOTO]);
}

DF_EXPORT_C GAME_INITIALIZE(GameInitialize)
{
    DF::GameMemory gameMemory {};
    gameMemory.permanent = DF::Platform::AllocateMemory(Megabytes(64));

    GameState* gameState = (GameState*)DF::ArenaPush(&gameMemory.permanent, sizeof(GameState));

    int totalFontSize = Kilobytes(256);

    gameState->fontsArena.base = DF::ArenaPush(&gameMemory.permanent, totalFontSize);
    gameState->fontsArena.size = totalFontSize;

    DF::Assets::Font* roboto = DF::Assets::LoadFont("resources/fonts/roboto_regular.json",
                                                    "resources/fonts/roboto_regular.png",
                                                    &gameState->fontsArena);

    if (roboto)
    {
        gameState->fonts[(int)Fonts::ROBOTO] = roboto;
    }

    int clayMemorySize        = Clay_MinMemorySize();
    gameState->clayArena.base = DF::ArenaPush(&gameMemory.permanent, clayMemorySize);
    gameState->clayArena.size = clayMemorySize;

    Clay_Arena clayMemory =
      Clay_CreateArenaWithCapacityAndMemory(clayMemorySize, gameState->clayArena.base);
    Clay_Initialize(
      clayMemory, Clay_Dimensions { 800, 600 }, Clay_ErrorHandler { HandleClayErrors });

    Clay_SetMeasureTextFunction(MeasureText, roboto);

    return gameMemory;
}

DF_EXPORT_C GAME_UPDATE(GameUpdate)
{
    GameState* gameState = (GameState*)gameMemory.permanent.base;

    float scrollSpeed       = 4;
    DF::Math::Vec2 scroll   = DF::Platform::GetScrollPos();
    DF::Math::Vec2 mousePos = DF::Platform::GetCursorPos();
    bool lmbPressed         = DF::Platform::MouseKeyPressed(DF::MouseKey::LEFT);

    Clay_SetPointerState(Clay_Vector2 { mousePos.x, mousePos.y }, lmbPressed);
    Clay_UpdateScrollContainers(true,
                                Clay_Vector2 {
                                  scroll.x * scrollSpeed,
                                  scroll.y * scrollSpeed,
                                },
                                dt);

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

                DF::Render::DrawText(gameState->fonts[(int)Fonts::ROBOTO],
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

////**************************** ENGINE PART ****************************//
//
// struct Arena
//{
//    void* base;
//    int offset;
//    int sizeTotal;
//    int sizeLeft;
//};
//
// void* ArenaPush(Arena* arena, int size)
//{
//    if (size > arena->sizeLeft)
//    {
//        // Error
//    }
//
//    void* res = (char*)arena->base + arena->offset;
//
//    arena->offset += size;
//    arena->sizeLeft -= size;
//
//    return res;
//}
//
// struct GameMemory
//{
//    Arena permanent;
//    Arena transient;
//};
//
// struct Font
//{
//    struct Glyph
//    {
//    };
//    int glyphCount;
//    Glyph* glyphs;
//};
//
// Font* LoadFont(const char* bitmap, const char* data, Arena* arena)
//{
//    // Load bitmap
//    // Load data
//
//    int fontSize = sizeof(Font) + sizeof(Glyph) * number_of_glyphs;
//    Font* font   = ArenaPush(arena, fontSize);
//
//    // Returns valid Font pointer or nullptr if failed
//    return font;
//}
//
////**************************** GAME PART ****************************//
//
// enum class Fonts
//{
//    ROBOTO,
//    SANS,
//    BITCOUNT,
//
//    TOTAL,
//};
//
// struct GameState
//{
//    Arena fontsArena;
//    Font* fonts[(int)Fonts::TOTAL];
//
//    Arena entities;
//    // ...
//};
//
// GameMemory GameInit()
//{
//    GameMemory gameMemory {};
//    gameMemory.permanent = AllocateMemory(size);
//
//    GameState* gameState = ArenaPush(&gameMemory.permanent, sizeof(GameState));
//
//    int totalFontSize = offset_from_base_to_place_first_font_data;
//
//    gameState->fontsArena.base      = ArenaPush(&gameMemory.permanent, totalFontSize);
//    gameState->fontsArena.offset    = 0;
//    gameState->fontsArena.sizeTotal = totalFontSize;
//    gameState->fontsArena.sizeLeft  = totalFontSize;
//
//    Font* roboto = LoadFont("roboto_bitmap", "roboto_data", &gameState->fontsArena);
//
//    if (roboto)
//    {
//        gameState->fonts[(int)Fonts::ROBOTO] = roboto;
//    }
//
//    // Usage example
//    int count = gameState->fonts[(int)Fonts::ROBOTO]->glyphCount;
//
//    // This returns gameMemory struct, so it will be saved inside main.exe
//    // and passed to the game on update.
//    return gameMemory;
//}
