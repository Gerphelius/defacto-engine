namespace SCP
{

struct TabData
{
    GameState* gameState;
    MenuTab tab;
};

static void SelectTab(Clay_ElementId elementId, Clay_PointerData pointerInfo, void* data)
{
    TabData* tabData = (TabData*)data;

    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        tabData->gameState->menuTab = tabData->tab;
    }
}

static Clay_RenderCommandArray CreateMenu(DF::GameMemory* gameMemory)
{
    GameState* gameState = (GameState*)gameMemory->permanent.base;

    Clay_BeginLayout();

    CLAY_AUTO_ID({
        .layout = {
                .sizing  = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
            },
            .backgroundColor = { 80, 40, 120, 256 },
        })
    {
        CLAY_AUTO_ID({
            .layout = {
                .sizing  = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(0) },
                .padding = { 16, 16, 8, 8 },
                .childGap        = 16,
                .childAlignment  = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER },
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
            },
            .backgroundColor = { 120, 120, 120, 256 },
        })
        {
            CLAY_AUTO_ID({
              .layout          = { .padding = { 16, 16, 8, 8 } },
              .backgroundColor = { 64.0f, (Clay_Hovered() ? 140.0f : 128.0f), 32.0f, 1.0f },
              .cornerRadius    = CLAY_CORNER_RADIUS(5),
            })
            {
                TabData* data = (TabData*)DF::ArenaPush(&gameMemory->transient, sizeof(TabData));

                data->gameState = gameState;
                data->tab       = MenuTab::PERSONNEL;
                Clay_OnHover(SelectTab, data);

                CLAY_TEXT(CLAY_STRING("Personnel"),
                          CLAY_TEXT_CONFIG({
                            .textColor = { 255, 255, 255, 255 },
                            .fontId    = 0,
                            .fontSize  = 16,
                          }));
            }

            CLAY_AUTO_ID({
              .layout          = { .padding = { 16, 16, 8, 8 } },
              .backgroundColor = { 64.0f, (Clay_Hovered() ? 140.0f : 128.0f), 32.0f, 1.0f },
              .cornerRadius    = CLAY_CORNER_RADIUS(5),
            })
            {
                TabData* data = (TabData*)DF::ArenaPush(&gameMemory->transient, sizeof(TabData));

                data->gameState = gameState;
                data->tab       = MenuTab::SQUADS;
                Clay_OnHover(SelectTab, data);

                CLAY_TEXT(CLAY_STRING("Squads"),
                          CLAY_TEXT_CONFIG({
                            .textColor = { 255, 255, 255, 255 },
                            .fontId    = 0,
                            .fontSize  = 16,
                          }));
            }
        }

        switch (gameState->menuTab)
        {
            case MenuTab::PERSONNEL:
            {
                CreateAgentsUI(gameMemory);

                break;
            }
            case MenuTab::SQUADS:
            {
                CreateSquadsUI(gameMemory);

                break;
            }
            default:
            {
                break;
            }
        }
    }

    return Clay_EndLayout();
}

} // namespace SCP
