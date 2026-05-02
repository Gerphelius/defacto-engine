namespace SCPX
{

static const char* squadNames[] { "ALPHA", "BRAVO",    "CHARLIE", "DELTA",   "ECHO",   "FOXTROT",
                                  "GOLF",  "HOTEL",    "INDIA",   "JULIETT", "KILO",   "LIMA",
                                  "MIKE",  "NOVEMBER", "OSCAR",   "PAPA",    "QUEBEC", "ROMEO" };

// TODO: Move Squad processing code from UI
static void CreateSquad(Clay_ElementId elementId, Clay_PointerData pointerInfo, void* data)
{
    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        GameState* gameState = (GameState*)data;

        if (gameState->squads.count < gameState->squads.maxCount)
        {
            Squad squad {};

            squad.id = gameState->squads.count;
            squad.agents.list =
              (Agent*)&gameState->squads.list[gameState->squads.count] + sizeof(Squad);
            squad.agents.maxCount = gameState->squads.maxAgentsPerSquad;
            char* squadName       = squad.name;

            for (const char* ch = squadNames[gameState->squads.count]; *ch; ++ch, ++squadName)
            {
                *squadName = *ch;
            }

            *squadName++ = '\0';

            gameState->squads.list[gameState->squads.count++] = squad;
        }
    }
}

struct SquadSelectData
{
    GameState* gameState;
    int squadId;
};

static void SelectSquad(Clay_ElementId elementId, Clay_PointerData pointerInfo, void* data)
{
    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        SquadSelectData* squadData = (SquadSelectData*)data;

        squadData->gameState->selectedSquadId = squadData->squadId;
    }
}

struct AgentAssignData
{
    GameState* gameState;
    int agentId;
};

static void AssignAgent(Clay_ElementId elementId, Clay_PointerData pointerInfo, void* data)
{
    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        AgentAssignData* agentData = (AgentAssignData*)data;

        if (agentData->gameState->selectedSquadId == -1) return;

        Squad* squad = &agentData->gameState->squads.list[agentData->gameState->selectedSquadId];
        Agent* agent = &agentData->gameState->agentsHired.list[agentData->agentId];

        // TODO: Make generic and move to global
        AddAgent(&squad->agents, *agent);

        squad->power += agent->power;

        Agent hired = agentData->gameState->agentsHired.list[agentData->agentId];
        Agent last =
          agentData->gameState->agentsHired.list[agentData->gameState->agentsHired.count - 1];

        agentData->gameState->agentsHired.list[agentData->agentId] = last;
        agentData->gameState->agentsHired.count--;
    }
}

static void CreateSquadsUI(DF::GameMemory* gameMemory)
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
        CLAY(CLAY_ID("Squads"),
            {
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                    .padding         = CLAY_PADDING_ALL(16),
                    .childGap        = 16,
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                },
                .backgroundColor = { 12, 41, 51, 255 },
                .clip            = { .vertical = true, .childOffset = Clay_GetScrollOffset() },
            })
        {
            CLAY_AUTO_ID({
                .layout = {
                    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(0) },
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                },
                })
            {
                CLAY_TEXT(CLAY_STRING("SQUADS"),
                          CLAY_TEXT_CONFIG({
                            .textColor = { 255, 255, 255, 255 },
                            .fontId    = 0,
                            .fontSize  = 20,
                          }));

                CLAY_AUTO_ID({ .layout = { .sizing = { .width  = CLAY_SIZING_GROW(0),
                                                       .height = CLAY_SIZING_GROW(0) } } })
                {
                }

                DF::String squadsNumFmt = DF::StrFormat(&gameMemory->transient,
                                                        "%i/%i",
                                                        gameState->squads.count,
                                                        gameState->squads.maxCount);
                Clay_String squadsNum   = {
                      .length = (int32_t)squadsNumFmt.length,
                      .chars  = squadsNumFmt.data,
                };

                CLAY_TEXT(squadsNum,
                          CLAY_TEXT_CONFIG({
                            .textColor = { 255, 255, 255, 255 },
                            .fontId    = 0,
                            .fontSize  = 20,
                          }));
            }

            for (int i = 0; i < gameState->squads.count; ++i)
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
                    SquadSelectData* data = (SquadSelectData*)DF::ArenaPush(
                      &gameMemory->transient, sizeof(SquadSelectData));

                    data->gameState = gameState;
                    data->squadId   = i;

                    Clay_OnHover(SelectSquad, data);

                    Clay_String name = {
                        .length = (int32_t)strlen(gameState->squads.list[i].name),
                        .chars  = gameState->squads.list[i].name,
                    };
                    DF::String powerFmt =
                      DF::StrFormat(&gameMemory->transient, "%.2f", gameState->squads.list[i].power);
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
                }
            }

            CLAY_AUTO_ID({
              .layout          = { .padding = { 16, 16, 8, 8 } },
              .backgroundColor = { 64.0f, (Clay_Hovered() ? 140.0f : 128.0f), 32.0f, 1.0f },
              .cornerRadius    = CLAY_CORNER_RADIUS(5),
            })
            {

                Clay_OnHover(CreateSquad, gameState);

                CLAY_TEXT(CLAY_STRING("CREATE SQUAD"),
                          CLAY_TEXT_CONFIG({
                            .textColor = { 255, 255, 255, 255 },
                            .fontId    = 0,
                            .fontSize  = 16,
                          }));
            }
        }

        CLAY(CLAY_ID("SquadInfo"),
            {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                        .padding = CLAY_PADDING_ALL(16),
                        .childGap = 16,
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    },
                    .backgroundColor = { 43, 1, 151, 255 },
            })
        {
            Squad* squad = &gameState->squads.list[gameState->selectedSquadId];

            Clay_String name = {
                .length = (int32_t)strlen(squad->name),
                .chars  = squad->name,
            };
            DF::String powerFmt = DF::StrFormat(&gameMemory->transient, "%i", squad->power);
            Clay_String power   = {
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

            CLAY(CLAY_ID("SquadMembers"),
                {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                        .padding         = CLAY_PADDING_ALL(16),
                        .childGap        = 16,
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    },
                    .backgroundColor = { 43, 81, 51, 255 },
                    .clip            = { .vertical = true, .childOffset = Clay_GetScrollOffset() },
                })
            {
                CLAY_AUTO_ID({
                .layout = {
                    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(0) },
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                },
                })
                {
                    CLAY_TEXT(CLAY_STRING("SQUAD MEMBERS"),
                              CLAY_TEXT_CONFIG({
                                .textColor = { 255, 255, 255, 255 },
                                .fontId    = 0,
                                .fontSize  = 14,
                              }));

                    CLAY_AUTO_ID({ .layout = { .sizing = { .width  = CLAY_SIZING_GROW(0),
                                                           .height = CLAY_SIZING_GROW(0) } } })
                    {
                    }

                    DF::String agentsNumFmt = DF::StrFormat(
                      &gameMemory->transient, "%i/%i", squad->agents.count, squad->agents.maxCount);
                    Clay_String agentsNum = {
                        .length = (int32_t)agentsNumFmt.length,
                        .chars  = agentsNumFmt.data,
                    };

                    CLAY_TEXT(agentsNum,
                              CLAY_TEXT_CONFIG({
                                .textColor = { 255, 255, 255, 255 },
                                .fontId    = 0,
                                .fontSize  = 14,
                              }));
                }

                for (int i = 0; i < squad->agents.count; ++i)
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
                            .length = (int32_t)strlen(squad->agents.list[i].name),
                            .chars  = squad->agents.list[i].name,
                        };
                        DF::String powerFmt =
                          DF::StrFormat(&gameMemory->transient, "%.2f", squad->agents.list[i].power);
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
                            /*AgentData* data =
                              (AgentData*)DF::ArenaPush(&gameMemory->transient, sizeof(AgentData));

                            data->agentIndex = i;
                            data->gameState  = gameState;

                            Clay_OnHover(FireAgent, data);

                            CLAY_TEXT(CLAY_STRING("X"),
                                      CLAY_TEXT_CONFIG({
                                        .textColor = { 255, 255, 255, 255 },
                                        .fontId    = 0,
                                        .fontSize  = 16,
                                      }));*/
                        }
                    }
                }
            }
        }

        CLAY(CLAY_ID("HiredAgents"),
                {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
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
                                .width  = CLAY_SIZING_FIXED(0),
                                .height = CLAY_SIZING_FIXED(0),
                            },
                            .padding = { 16, 16, 8, 8 },
                            .childAlignment  = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER },
                      },
                      .backgroundColor = { 64.0f, (Clay_Hovered() ? 140.0f : 128.0f), 32.0f, 1.0f },
                      .cornerRadius    = CLAY_CORNER_RADIUS(5),
                    })
                    {
                        AgentAssignData* data = (AgentAssignData*)DF::ArenaPush(
                          &gameMemory->transient, sizeof(AgentAssignData));

                        data->agentId   = i;
                        data->gameState = gameState;

                        Clay_OnHover(AssignAgent, data);

                        CLAY_TEXT(CLAY_STRING("ASSIGN"),
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
