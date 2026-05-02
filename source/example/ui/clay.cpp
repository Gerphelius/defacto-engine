#pragma warning(push, 0)
#define CLAY_IMPLEMENTATION
#include "clay.h"
#pragma warning(pop)

namespace SCPX
{
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
    DF::Font* font = (DF::Font*)userData;

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

static void InitClay(GameState* gameState)
{
    Clay_Arena clayMemory =
      Clay_CreateArenaWithCapacityAndMemory(gameState->clayArena.size, gameState->clayArena.base);
    Clay_Initialize(
      clayMemory, Clay_Dimensions { 800, 600 }, Clay_ErrorHandler { HandleClayErrors });
    Clay_SetMeasureTextFunction(MeasureText, gameState->fonts[(int)Fonts::ROBOTO]);
}

static void RenderUI(DF::GameMemory* gameMemory, float dt)
{
    GameState* gameState = (GameState*)gameMemory->permanent.base;
    DF::Font* roboto     = gameState->fonts[(int)Fonts::ROBOTO];


    float scrollSpeed = 4;
    DF::Vec2 scroll   = DF::Platform::GetScrollPos();
    DF::Vec2 mousePos = DF::Platform::GetCursorPos();
    bool lmbPressed   = DF::Platform::MouseKeyPressed(DF::MouseKey::LEFT);

    Clay_SetPointerState(Clay_Vector2 { mousePos.x, mousePos.y }, lmbPressed);
    Clay_UpdateScrollContainers(true,
                                Clay_Vector2 {
                                  scroll.x * scrollSpeed,
                                  scroll.y * scrollSpeed,
                                },
                                dt);

    DF::Size fbSize = DF::Platform::GetFramebufferSize();

    Clay_SetLayoutDimensions(Clay_Dimensions { (float)fbSize.width, (float)fbSize.height });

    Clay_RenderCommandArray renderCommands = CreateMenu(gameMemory);

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

                DF::Render::DrawText(roboto,
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

} // namespace SCP
