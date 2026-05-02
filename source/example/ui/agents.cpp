namespace SCPX
{

static const char* names[]    = { "James",  "Oliver", "William", "Henry",   "Arthur",
                                  "George", "Thomas", "Edward",  "Charles", "Frederick" };
static const char* surnames[] = { "Smith",  "Johnson", "Williams", "Brown",  "Jones",
                                  "Garcia", "Miller",  "Davis",    "Wilson", "Taylor" };

// TODO: Move Agent processing from UI code
static Agent GenerateRandomAgent()
{
    Agent agent {};

    const char* name    = names[DF::RandomInt(0, 9)];
    const char* surname = surnames[DF::RandomInt(0, 9)];

    char* agentName = agent.name;

    for (const char* ch = name; *ch; ++ch, ++agentName)
    {
        *agentName = *ch;
    }

    *agentName++ = ' ';

    for (const char* ch = surname; *ch; ++ch, ++agentName)
    {
        *agentName = *ch;
    }

    *agentName++ = '\0';

    agent.power = (float)DF::RandomInt(20, 50);

    return agent;
}

static void AddAgent(Agents* agents, Agent agent)
{
    if (agents->count < agents->maxCount)
    {
        agents->list[agents->count++] = agent;
    }
}

struct AgentData
{
    int agentIndex;
    GameState* gameState;
};

static void HireAgent(Clay_ElementId elementId, Clay_PointerData pointerInfo, void* data)
{
    AgentData* agentData = (AgentData*)data;

    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Agent hired = agentData->gameState->agentsForHire.list[agentData->agentIndex];
        Agent last =
          agentData->gameState->agentsForHire.list[agentData->gameState->agentsForHire.count - 1];

        agentData->gameState->agentsForHire.list[agentData->agentIndex] = last;
        agentData->gameState->agentsForHire.count--;

        AddAgent(&agentData->gameState->agentsHired, hired);
        hired.hired = true;
    }
}

static void FireAgent(Clay_ElementId elementId, Clay_PointerData pointerInfo, void* data)
{
    AgentData* agentData = (AgentData*)data;

    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Agent hired = agentData->gameState->agentsHired.list[agentData->agentIndex];
        Agent last =
          agentData->gameState->agentsHired.list[agentData->gameState->agentsHired.count - 1];

        agentData->gameState->agentsHired.list[agentData->agentIndex] = last;
        agentData->gameState->agentsHired.count--;
    }
}

static void CreateAgentsUI(DF::GameMemory* gameMemory)
{
    GameState* gameState = (GameState*)gameMemory->permanent.base;

    CLAY(CLAY_ID("OuterContainer"),
        {
            .layout          = {
                .sizing          = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                .padding         = CLAY_PADDING_ALL(16),
                .childGap        = 16,
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
            },
            .backgroundColor = { 43, 41, 51, 255 },
        })
    {
        CLAY(CLAY_ID("AgentsForHire"),
            {
                .layout = {
                    .sizing = { .width = CLAY_SIZING_PERCENT(0.5), .height = CLAY_SIZING_GROW(0) },
                    .padding         = CLAY_PADDING_ALL(16),
                    .childGap        = 16,
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                },
                .backgroundColor = { 143, 41, 51, 255 },
                .clip            = { .vertical = true, .childOffset = Clay_GetScrollOffset() },
            })
        {
            CLAY_TEXT(CLAY_STRING("AVAILABLE AGENTS"),
                      CLAY_TEXT_CONFIG({
                        .textColor = { 255, 255, 255, 255 },
                        .fontId    = 0,
                        .fontSize  = 20,
                      }));

            for (int i = 0; i < gameState->agentsForHire.count; ++i)
            {
                CLAY_AUTO_ID(
                {
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
                    Clay_String name = {
                        .length = (int32_t)strlen(gameState->agentsForHire.list[i].name),
                        .chars  = gameState->agentsForHire.list[i].name,
                    };
                    DF::String powerFmt = DF::StrFormat(
                      &gameMemory->transient, "%.2f", gameState->agentsForHire.list[i].power);
                    Clay_String power = {
                        .length = (int32_t)powerFmt.length,
                        .chars  = powerFmt.data,
                    };

                    CLAY_TEXT(name,
                              CLAY_TEXT_CONFIG({
                                .textColor = { 255, 255, 255, 255 },
                                .fontId    = 0,
                                .fontSize  = 16,
                              }));

                    CLAY_TEXT(power,
                              CLAY_TEXT_CONFIG({
                                .textColor = { 255, 255, 255, 255 },
                                .fontId    = 0,
                                .fontSize  = 16,
                              }));

                    CLAY_AUTO_ID({ .layout = { .sizing = { .width  = CLAY_SIZING_GROW(0),
                                                           .height = CLAY_SIZING_GROW(0) } } })
                    {
                    }

                    CLAY_AUTO_ID({
                      .layout          = { .padding = { 16, 16, 8, 8 } },
                      .backgroundColor = { 64.0f, (Clay_Hovered() ? 140.0f : 128.0f), 32.0f, 1.0f },
                      .cornerRadius    = CLAY_CORNER_RADIUS(5),
                    })
                    {
                        AgentData* data =
                          (AgentData*)DF::ArenaPush(&gameMemory->transient, sizeof(AgentData));

                        data->agentIndex = i;
                        data->gameState  = gameState;

                        Clay_OnHover(HireAgent, data);

                        CLAY_TEXT(CLAY_STRING("Hire"),
                                  CLAY_TEXT_CONFIG({
                                    .textColor = { 255, 255, 255, 255 },
                                    .fontId    = 0,
                                    .fontSize  = 16,
                                  }));
                    }
                }
            }
        }

        CLAY(CLAY_ID("HiredAgents"),
                {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_PERCENT(0.5), .height = CLAY_SIZING_GROW(0) },
                        .padding         = CLAY_PADDING_ALL(16),
                        .childGap        = 16,
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    },
                    .backgroundColor = { 43, 81, 51, 255 },
                    .clip            = { .vertical = true, .childOffset = Clay_GetScrollOffset() },
                })
        {
            CLAY_TEXT(CLAY_STRING("HIRED AGENTS"),
                      CLAY_TEXT_CONFIG({
                        .textColor = { 255, 255, 255, 255 },
                        .fontId    = 0,
                        .fontSize  = 20,
                      }));

            for (int i = 0; i < gameState->agentsHired.count; ++i)
            {
                CLAY_AUTO_ID(
                {
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
                    Clay_String name = {
                        .length = (int32_t)strlen(gameState->agentsHired.list[i].name),
                        .chars  = gameState->agentsHired.list[i].name,
                    };
                    DF::String powerFmt = DF::StrFormat(
                      &gameMemory->transient, "%.2f", gameState->agentsHired.list[i].power);
                    Clay_String power = {
                        .length = (int32_t)powerFmt.length,
                        .chars  = powerFmt.data,
                    };

                    CLAY_TEXT(name,
                              CLAY_TEXT_CONFIG({
                                .textColor = { 255, 255, 255, 255 },
                                .fontId    = 0,
                                .fontSize  = 16,
                              }));

                    CLAY_TEXT(power,
                              CLAY_TEXT_CONFIG({
                                .textColor = { 255, 255, 255, 255 },
                                .fontId    = 0,
                                .fontSize  = 16,
                              }));

                    CLAY_AUTO_ID({ .layout = { .sizing = { .width  = CLAY_SIZING_GROW(0),
                                                           .height = CLAY_SIZING_GROW(0) } } })
                    {
                    }

                    CLAY_AUTO_ID({
                      .layout          = {
                            .sizing = {
                                .width  = CLAY_SIZING_FIXED(34),
                                .height = CLAY_SIZING_FIXED(34),
                            },
                            .childAlignment  = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER },
                      },
                      .backgroundColor = { 64.0f, (Clay_Hovered() ? 140.0f : 128.0f), 32.0f, 1.0f },
                      .cornerRadius    = CLAY_CORNER_RADIUS(5),
                    })
                    {
                        AgentData* data =
                          (AgentData*)DF::ArenaPush(&gameMemory->transient, sizeof(AgentData));

                        data->agentIndex = i;
                        data->gameState  = gameState;

                        Clay_OnHover(FireAgent, data);

                        CLAY_TEXT(CLAY_STRING("X"),
                                  CLAY_TEXT_CONFIG({
                                    .textColor = { 255, 255, 255, 255 },
                                    .fontId    = 0,
                                    .fontSize  = 16,
                                  }));
                    }
                }
            }
        }
    }
}

} // namespace SCP
