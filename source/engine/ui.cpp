#include <cstdio>

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

void Initialize()
{
    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemory =
      Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));

    Clay_Initialize(
      clayMemory, Clay_Dimensions { 800, 600 }, Clay_ErrorHandler { HandleClayErrors });
    
    // renderCommands = GetTestLayout();
}

void Render(int width, int height)
{
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
                Clay_Color c     = renderCommand->renderData.text.textColor;
                uint16_t font_id = renderCommand->renderData.text.fontId;

                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE:
            {
                Clay_RectangleRenderData rrd = renderCommand->renderData.rectangle;

                DF::RENDER::DrawQuad({ boundingBox.x, boundingBox.y },
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
                printf("Unhandled render command %d\r\n", renderCommand->commandType);
                break;
        }
    }
}
} // namespace DF::UI
