namespace SCPX
{

static DF::String MissionStageToStr(Mission::Stage::Type stage)
{
    switch (stage)
    {
        case Mission::Stage::INVESTIGATION:
        {
            return DF::String("INVESTIGATION");
        }
        case Mission::Stage::SURVEILLANCE:
        {
            return DF::String("SURVEILLANCE");
        }
        case Mission::Stage::CONTAINMENT:
        {
            return DF::String("CONTAINMENT");
        }
        default:
        {
            return DF::String("UNKNOWN");
        }
    }
}

static DF::String ContainmentToStr(SCP::Containment containment)
{
    switch (containment)
    {
        case SCP::Containment::SAFE:
        {
            return DF::String("SAFE");
        }
        case SCP::Containment::EUCLID:
        {
            return DF::String("EUCLID");
        }
        case SCP::Containment::KETER:
        {
            return DF::String("KETER");
        }
        default:
        {
            return DF::String("UNKNOWN");
        }
    }
}

struct MissionSelectData
{
    GameState* gameState;
    int missionId;
};

static void SelectMission(Clay_ElementId elementId, Clay_PointerData pointerInfo, void* data)
{
    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        MissionSelectData* squadData = (MissionSelectData*)data;

        squadData->gameState->selectedMissionId = squadData->missionId;
    }
}

struct SquadAssignData
{
    GameState* gameState;
    int missionId;
};

static void ShowSquadAssign(Clay_ElementId elementId, Clay_PointerData pointerInfo, void* data)
{
    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        SquadAssignData* squadAssignData = (SquadAssignData*)data;

        squadAssignData->gameState->showMissionSquadAssign = squadAssignData->missionId >= 0;
    }
}

static void DismissMissionSquad(Clay_ElementId elementId, Clay_PointerData pointerInfo, void* data)
{
    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        GameState* gameState = (GameState*)data;
        Mission* mission     = &gameState->missions.list[gameState->selectedMissionId];

        mission->squadId = -1;
    }
}

struct AssignSquadData
{
    GameState* gameState;
    int squadId;
};

static void AssignMissionSquad(Clay_ElementId elementId, Clay_PointerData pointerInfo, void* data)
{
    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        AssignSquadData* assignSquadData = (AssignSquadData*)data;
        Mission* mission =
          &assignSquadData->gameState->missions.list[assignSquadData->gameState->selectedMissionId];
        Squad* squad = &assignSquadData->gameState->squads.list[assignSquadData->squadId];

        if (squad->missionId >= 0)
        {
            assignSquadData->gameState->missions.list[squad->missionId].squadId = -1;
        }

        mission->squadId = assignSquadData->squadId;
        squad->missionId = assignSquadData->gameState->selectedMissionId;
        assignSquadData->gameState->showMissionSquadAssign = false;
    }
}

static void GoToSquadsTab(Clay_ElementId elementId, Clay_PointerData pointerInfo, void* data)
{
    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        GameState* gameState = (GameState*)data;

        gameState->menuTab = MenuTab::SQUADS;
    }
}

static void MissionNextStage(Clay_ElementId elementId, Clay_PointerData pointerInfo, void* data)
{
    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        GameState* gameState = (GameState*)data;
        Mission* mission     = &gameState->missions.list[gameState->selectedMissionId];

        mission->stage.type     = (Mission::Stage::Type)(mission->stage.type + 1);
        mission->stage.progress = 0.0f;
    }
}

static void CreateMissionsUI(DF::GameMemory* gameMemory)
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
        CLAY(CLAY_ID("Missions"),
            {
                .layout = {
                    .sizing = { .width = CLAY_SIZING_FIXED(0), .height = CLAY_SIZING_GROW(0) },
                    .padding         = CLAY_PADDING_ALL(16),
                    .childGap        = 16,
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                },
                .backgroundColor = { 143, 41, 51, 255 },
                .clip            = { .vertical = true, .childOffset = Clay_GetScrollOffset() },
            })
        {
            CLAY_TEXT(CLAY_STRING("MISSIONS"),
                      CLAY_TEXT_CONFIG({
                        .textColor = { 255, 255, 255, 255 },
                        .fontId    = 0,
                        .fontSize  = 20,
                      }));

            for (int i = 0; i < gameState->missions.count; ++i)
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
                    .backgroundColor = { 120, 10, 40, 256 },
                })
                {
                    MissionSelectData* data = (MissionSelectData*)DF::ArenaPush(
                      &gameMemory->transient, sizeof(MissionSelectData));

                    data->gameState = gameState;
                    data->missionId = i;

                    Clay_OnHover(SelectMission, data);

                    Clay_String name = {
                        .length = (int)strlen(gameState->missions.list[i].name),
                        .chars  = gameState->missions.list[i].name,
                    };
                    DF::String stage = MissionStageToStr(gameState->missions.list[i].stage.type);
                    Clay_String stageStr = {
                        .length = (int32_t)stage.length,
                        .chars  = stage.data,
                    };

                    CLAY_TEXT(name,
                              CLAY_TEXT_CONFIG({
                                .textColor = { 255, 255, 255, 255 },
                                .fontId    = 0,
                                .fontSize  = 16,
                              }));

                    CLAY_TEXT(stageStr,
                              CLAY_TEXT_CONFIG({
                                .textColor = { 255, 255, 255, 255 },
                                .fontId    = 0,
                                .fontSize  = 16,
                              }));
                }
            }
        }

        if (gameState->selectedMissionId >= 0)
        {
            CLAY(CLAY_ID("mission_details"),
                {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_FIXED(0), .height = CLAY_SIZING_GROW(0) },
                        .padding         = CLAY_PADDING_ALL(16),
                        .childGap        = 16,
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    },
                    .backgroundColor = { 22, 41, 151, 255 },
                })
            {
                Mission* mission = &gameState->missions.list[gameState->selectedMissionId];

                Clay_String name = {
                    .length = (int)strlen(mission->name),
                    .chars  = mission->name,
                };

                CLAY_TEXT(name,
                          CLAY_TEXT_CONFIG({
                            .textColor = { 255, 255, 255, 255 },
                            .fontId    = 0,
                            .fontSize  = 16,
                          }));

                CLAY(CLAY_ID("stage"),
                        {
                            .layout = {
                                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(0) },
                                .childGap = 16,
                                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                            },
                        })
                {
                    CLAY_TEXT(CLAY_STRING("STAGE:"),
                              CLAY_TEXT_CONFIG({
                                .textColor = { 255, 255, 255, 255 },
                                .fontId    = 0,
                                .fontSize  = 16,
                              }));

                    DF::String stage     = MissionStageToStr(mission->stage.type);
                    Clay_String stageStr = {
                        .length = (int32_t)stage.length,
                        .chars  = stage.data,
                    };

                    CLAY_TEXT(stageStr,
                              CLAY_TEXT_CONFIG({
                                .textColor = { 255, 255, 255, 255 },
                                .fontId    = 0,
                                .fontSize  = 16,
                              }));
                }

                CLAY(CLAY_ID("stage_progress"),
                    {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(0) },
                            .childGap        = 16,
                            .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        },
                    })
                {
                    CLAY_TEXT(CLAY_STRING("STAGE PROGRESS:"),
                              CLAY_TEXT_CONFIG({
                                .textColor = { 255, 255, 255, 255 },
                                .fontId    = 0,
                                .fontSize  = 16,
                              }));

                    float progress =
                      mission->stage.progress < 100.0f ? mission->stage.progress : 100.0f;

                    if (mission->stage.progress >= 100.0f &&
                        mission->stage.type != Mission::Stage::CONTAINMENT)
                    {
                        CLAY(CLAY_ID("next_stage_btn"),
                             {
                               .layout          = { .padding = { 16, 16, 8, 8 } },
                               .backgroundColor = { 64.0f,
                                                    (Clay_Hovered() ? 140.0f : 128.0f),
                                                    32.0f,
                                                    1.0f },
                               .cornerRadius    = CLAY_CORNER_RADIUS(5),
                             })
                        {
                            Clay_OnHover(MissionNextStage, gameState);

                            CLAY_TEXT(CLAY_STRING("CONTINUE"),
                                      CLAY_TEXT_CONFIG({
                                        .textColor = { 255, 255, 255, 255 },
                                        .fontId    = 0,
                                        .fontSize  = 16,
                                      }));
                        }
                    }
                    else
                    {
                        CLAY(CLAY_ID("progress_bar"),
                            {
                                .layout = {
                                    .sizing = {.width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(20) },
                                    .padding = CLAY_PADDING_ALL(2),
                                    .childGap = 2,
                                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                },
                                .backgroundColor = { 151, 41, 22, 255 },
                            })
                        {

                            CLAY_AUTO_ID({
                                    .layout = {
                                        .sizing = { .width = CLAY_SIZING_PERCENT(progress / 100.0f), .height = CLAY_SIZING_GROW(0) },
                                    },
                                    .backgroundColor = { 22, 141, 3, 255 },
                                })
                            {
                            }
                        }

                        DF::String progressFmt =
                          DF::StrFormat(&gameMemory->transient, "%.2f", progress);
                        Clay_String progressStr = {
                            .length = (int32_t)progressFmt.length,
                            .chars  = progressFmt.data,
                        };

                        CLAY_TEXT(progressStr,
                                  CLAY_TEXT_CONFIG({
                                    .textColor = { 255, 255, 255, 255 },
                                    .fontId    = 0,
                                    .fontSize  = 16,
                                  }));
                    }
                }

                CLAY(CLAY_ID("scp_info"),
                    {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(0) },
                            .childGap        = 16,
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        },
                    })
                {
                    const char* name                 = "UNKNOWN";
                    SCP::Containment containmentType = SCP::Containment::UNKNOWN;

                    if (!mission->stage.type == Mission::Stage::INVESTIGATION ||
                        mission->stage.progress >= 100.0f)
                    {
                        name            = gameState->scps.list[mission->scpId].name;
                        containmentType = gameState->scps.list[mission->scpId].containment;
                    }

                    Clay_String nameStr = {
                        .length = (int32_t)strlen(name),
                        .chars  = name,
                    };

                    CLAY_AUTO_ID(
                    {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(0) },
                            .childGap        = 16,
                            .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        },
                    })
                    {
                        CLAY_TEXT(CLAY_STRING("SCP:"),
                                  CLAY_TEXT_CONFIG({
                                    .textColor = { 255, 255, 255, 255 },
                                    .fontId    = 0,
                                    .fontSize  = 16,
                                  }));

                        CLAY_TEXT(nameStr,
                                  CLAY_TEXT_CONFIG({
                                    .textColor = { 255, 255, 255, 255 },
                                    .fontId    = 0,
                                    .fontSize  = 16,
                                  }));
                    }

                    DF::String containment = ContainmentToStr(containmentType);

                    Clay_String containmentStr = {
                        .length = (int32_t)containment.length,
                        .chars  = containment.data,
                    };

                    CLAY_AUTO_ID(
                    {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(0) },
                            .childGap        = 16,
                            .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        },
                    })
                    {
                        CLAY_TEXT(CLAY_STRING("CONTAINMENT CLASS:"),
                                  CLAY_TEXT_CONFIG({
                                    .textColor = { 255, 255, 255, 255 },
                                    .fontId    = 0,
                                    .fontSize  = 16,
                                  }));

                        CLAY_TEXT(containmentStr,
                                  CLAY_TEXT_CONFIG({
                                    .textColor = { 255, 255, 255, 255 },
                                    .fontId    = 0,
                                    .fontSize  = 16,
                                  }));
                    }
                }

                CLAY(CLAY_ID("squad_info"), {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(0) },
                            .padding = { 0, 0, 16, 16 },
                            .childGap = 16,
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        },
                    })
                {
                    if (mission->squadId >= 0)
                    {
                        CLAY_AUTO_ID({
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(0) },
                                    .childGap = 4,
                                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                },
                            })
                        {
                            Squad* missionSquad = &gameState->squads.list[mission->squadId];

                            Clay_String squadName = {
                                .length = (int32_t)strlen(missionSquad->name),
                                .chars  = missionSquad->name,
                            };

                            DF::String squadPowerFmt =
                              DF::StrFormat(&gameMemory->transient, "%.2f", missionSquad->power);
                            Clay_String squadPower = {
                                .length = squadPowerFmt.length,
                                .chars  = squadPowerFmt.data,
                            };

                            CLAY_TEXT(squadName,
                                      CLAY_TEXT_CONFIG({
                                        .textColor = { 255, 255, 255, 255 },
                                        .fontId    = 0,
                                        .fontSize  = 16,
                                      }));
                            CLAY_TEXT(squadPower,
                                      CLAY_TEXT_CONFIG({
                                        .textColor = { 255, 255, 255, 255 },
                                        .fontId    = 0,
                                        .fontSize  = 16,
                                      }));
                        }
                    }
                    else
                    {
                        CLAY_AUTO_ID({
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(0) },
                                    .childGap = 4,
                                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                },
                            })
                        {
                            CLAY_TEXT(CLAY_STRING("SQUAD NOT ASSIGNED"),
                                      CLAY_TEXT_CONFIG({
                                        .textColor = { 255, 255, 255, 255 },
                                        .fontId    = 0,
                                        .fontSize  = 16,
                                      }));
                            CLAY_TEXT(
                              CLAY_STRING("Assign squad to start progressing through the mission"),
                              CLAY_TEXT_CONFIG({
                                .textColor = { 255, 255, 255, 255 },
                                .fontId    = 0,
                                .fontSize  = 12,
                              }));
                        }
                    }

                    CLAY_AUTO_ID({
                            .layout = {
                                .childGap = 16,
                            },
                        })
                    {
                        if (!gameState->showMissionSquadAssign)
                        {
                            CLAY(CLAY_ID("assign_squad_btn"),
                                 {
                                   .layout          = { .padding = { 16, 16, 8, 8 } },
                                   .backgroundColor = { 64.0f,
                                                        (Clay_Hovered() ? 140.0f : 128.0f),
                                                        32.0f,
                                                        1.0f },
                                   .cornerRadius    = CLAY_CORNER_RADIUS(5),
                                 })
                            {
                                MissionSelectData* data = (MissionSelectData*)DF::ArenaPush(
                                  &gameMemory->transient, sizeof(MissionSelectData));

                                data->gameState = gameState;
                                data->missionId = mission->id;
                                Clay_OnHover(ShowSquadAssign, data);

                                Clay_String assignSquadBtn = {
                                    .length = (int32_t)strlen("ASSIGN SQUAD"),
                                    .chars  = "ASSIGN SQUAD",
                                };

                                if (mission->squadId >= 0)
                                {
                                    assignSquadBtn = {
                                        .length = (int32_t)strlen("CHANGE SQUAD"),
                                        .chars  = "CHANGE SQUAD",
                                    };
                                }

                                CLAY_TEXT(assignSquadBtn,
                                          CLAY_TEXT_CONFIG({
                                            .textColor = { 255, 255, 255, 255 },
                                            .fontId    = 0,
                                            .fontSize  = 16,
                                          }));
                            }
                        }
                        else
                        {
                            CLAY(CLAY_ID("cancel_squad_assign_btn"),
                                 {
                                   .layout          = { .padding = { 16, 16, 8, 8 } },
                                   .backgroundColor = { 64.0f,
                                                        (Clay_Hovered() ? 140.0f : 128.0f),
                                                        32.0f,
                                                        1.0f },
                                   .cornerRadius    = CLAY_CORNER_RADIUS(5),
                                 })
                            {
                                MissionSelectData* data = (MissionSelectData*)DF::ArenaPush(
                                  &gameMemory->transient, sizeof(MissionSelectData));

                                data->gameState = gameState;
                                data->missionId = -1;
                                Clay_OnHover(ShowSquadAssign, data);

                                CLAY_TEXT(CLAY_STRING("CANCEL"),
                                          CLAY_TEXT_CONFIG({
                                            .textColor = { 255, 255, 255, 255 },
                                            .fontId    = 0,
                                            .fontSize  = 16,
                                          }));
                            }
                        }

                        if (mission->squadId >= 0)
                        {
                            CLAY(CLAY_ID("dismiss_squad_btn"),
                                 {
                                   .layout          = { .padding = { 16, 16, 8, 8 } },
                                   .backgroundColor = { 64.0f,
                                                        (Clay_Hovered() ? 140.0f : 128.0f),
                                                        32.0f,
                                                        1.0f },
                                   .cornerRadius    = CLAY_CORNER_RADIUS(5),
                                 })
                            {
                                Clay_OnHover(DismissMissionSquad, gameState);

                                CLAY_TEXT(CLAY_STRING("DISMISS SQUAD"),
                                          CLAY_TEXT_CONFIG({
                                            .textColor = { 255, 255, 255, 255 },
                                            .fontId    = 0,
                                            .fontSize  = 16,
                                          }));
                            }
                        }
                    }
                }

                if (gameState->showMissionSquadAssign)
                {
                    if (gameState->squads.count)
                    {
                        CLAY_TEXT(CLAY_STRING("AVAILABLE SQUADS"),
                                  CLAY_TEXT_CONFIG({
                                    .textColor = { 255, 255, 255, 255 },
                                    .fontId    = 0,
                                    .fontSize  = 16,
                                  }));

                        CLAY(CLAY_ID("squads_list"), {
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                                    .childGap = 16,
                                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                },
                                .backgroundColor = { 22, 41, 151, 255 },
                                .clip            = { .vertical = true, .childOffset = Clay_GetScrollOffset() },
                            })
                        {

                            for (int squadIdx = 0; squadIdx < gameState->squads.count; ++squadIdx)
                            {
                                Squad* squad = &gameState->squads.list[squadIdx];

                                Clay_String squadName = {
                                    .length = (int32_t)strlen(squad->name),
                                    .chars  = squad->name,
                                };
                                DF::String powerFmt =
                                  DF::StrFormat(&gameMemory->transient, "%.2f", squad->power);
                                Clay_String squadPower = {
                                    .length = (int32_t)powerFmt.length,
                                    .chars  = powerFmt.data,
                                };

                                CLAY_AUTO_ID({ .layout = {
                                                 .sizing          = { .width  = CLAY_SIZING_GROW(0),
                                                                      .height = CLAY_SIZING_FIXED(0) },
                                                 .childGap        = 16,
                                                 .childAlignment  = { .y = CLAY_ALIGN_Y_CENTER },
                                                 .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                               } })
                                {
                                    CLAY_TEXT(squadName,
                                              CLAY_TEXT_CONFIG({
                                                .textColor = { 255, 255, 255, 255 },
                                                .fontId    = 0,
                                                .fontSize  = 16,
                                              }));

                                    CLAY_TEXT(squadPower,
                                              CLAY_TEXT_CONFIG({
                                                .textColor = { 255, 255, 255, 255 },
                                                .fontId    = 0,
                                                .fontSize  = 16,
                                              }));

                                    CLAY_AUTO_ID(
                                      { .layout = { .sizing = { .width  = CLAY_SIZING_GROW(0),
                                                                .height = CLAY_SIZING_GROW(0) } } })
                                    {
                                    }

                                    CLAY_AUTO_ID({
                                      .layout          = { .padding = { 16, 16, 8, 8 } },
                                      .backgroundColor = { 64.0f,
                                                           (Clay_Hovered() ? 140.0f : 128.0f),
                                                           32.0f,
                                                           1.0f },
                                      .cornerRadius    = CLAY_CORNER_RADIUS(5),
                                    })
                                    {
                                        AssignSquadData* data = (AssignSquadData*)DF::ArenaPush(
                                          &gameMemory->transient, sizeof(AssignSquadData));

                                        data->gameState = gameState;
                                        data->squadId   = squadIdx;

                                        Clay_OnHover(AssignMissionSquad, data);

                                        CLAY_TEXT(CLAY_STRING("ASSIGN"),
                                                  CLAY_TEXT_CONFIG({
                                                    .textColor = { 255, 255, 255, 255 },
                                                    .fontId    = 0,
                                                    .fontSize  = 14,
                                                  }));
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        CLAY_TEXT(CLAY_STRING("NO AVAILABLE SQUADS"),
                                  CLAY_TEXT_CONFIG({
                                    .textColor = { 255, 255, 255, 255 },
                                    .fontId    = 0,
                                    .fontSize  = 16,
                                  }));

                        CLAY(CLAY_ID("create_squad_btn"),
                             {
                               .layout          = { .padding = { 16, 16, 8, 8 } },
                               .backgroundColor = { 64.0f,
                                                    (Clay_Hovered() ? 140.0f : 128.0f),
                                                    32.0f,
                                                    1.0f },
                               .cornerRadius    = CLAY_CORNER_RADIUS(5),
                             })
                        {
                            Clay_OnHover(GoToSquadsTab, gameState);

                            CLAY_TEXT(CLAY_STRING("CREATE SQUAD"),
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
}

} // namespace SCPX
