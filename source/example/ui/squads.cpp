namespace SCPX
{

static const char* squadNames[] { "ALPHA", "BRAVO",    "CHARLIE", "DELTA",   "ECHO",   "FOXTROT",
                                  "GOLF",  "HOTEL",    "INDIA",   "JULIETT", "KILO",   "LIMA",
                                  "MIKE",  "NOVEMBER", "OSCAR",   "PAPA",    "QUEBEC", "ROMEO" };

static void CreateSquad(Clay_ElementId elementId, Clay_PointerData pointerInfo, void* data)
{
    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        GameState* gameState = (GameState*)data;

        if (gameState->squads.count < gameState->squads.maxCount)
        {
            Squad squad {};

            squad.id = gameState->squads.count;
            squad.membersMax = gameState->maxMembersPerSquad;

            squad.assaultIds.data =
              (int*)&gameState->squads.data[gameState->squads.count] + sizeof(Squad);
            squad.assaultIds.maxCount = gameState->maxMembersPerSquad;
            squad.scientistIds.data   = (int*)&gameState->squads.data[gameState->squads.count] +
                                      sizeof(Squad) + gameState->maxMembersPerSquad * 1;
            squad.scientistIds.maxCount = gameState->maxMembersPerSquad;
            squad.reconIds.data         = (int*)&gameState->squads.data[gameState->squads.count] +
                                  sizeof(Squad) + gameState->maxMembersPerSquad * 2;
            squad.reconIds.maxCount = gameState->maxMembersPerSquad;
            squad.supportIds.data   = (int*)&gameState->squads.data[gameState->squads.count] +
                                    sizeof(Squad) + gameState->maxMembersPerSquad * 3;
            squad.supportIds.maxCount = gameState->maxMembersPerSquad;

            char* squadName = squad.name;

            for (const char* ch = squadNames[gameState->squads.count]; *ch; ++ch, ++squadName)
            {
                *squadName = *ch;
            }

            *squadName++ = '\0';

            gameState->squads.data[gameState->squads.count++] = squad;
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

struct MemberData
{
    GameState* gameState;
    int id;
    EmployeeType type;
};

void SquadMemberAdd(Squad* squad, Assault* member)
{
    if (squad->membersCurrent >= squad->membersMax) return;

    ListAdd(&squad->assaultIds, member->id);

    squad->suppression += member->suppression;

    squad->membersCurrent++;
}

void SquadMemberAdd(Squad* squad, Scientist* member)
{
    if (squad->membersCurrent >= squad->membersMax) return;

    ListAdd(&squad->assaultIds, member->id);

    squad->biology += member->biology;
    squad->chemistry += member->chemistry;
    squad->mechanics += member->mechanics;
    squad->physics += member->physics;
    squad->electronics += member->electronics;

    squad->membersCurrent++;
}

void SquadMemberAdd(Squad* squad, Recon* member)
{
    if (squad->membersCurrent >= squad->membersMax) return;

    ListAdd(&squad->assaultIds, member->id);

    squad->detection += member->detection;
    squad->stealth += member->stealth;

    squad->membersCurrent++;
}

void SquadMemberAdd(Squad* squad, Support* member)
{
    if (squad->membersCurrent >= squad->membersMax) return;

    ListAdd(&squad->assaultIds, member->id);

    squad->medical += member->medical;
    squad->logistics += member->logistics;
    squad->secrecy += member->secrecy;

    squad->membersCurrent++;
}

static void AssignMember(Clay_ElementId elementId, Clay_PointerData pointerInfo, void* data)
{
    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        MemberData* memberData = (MemberData*)data;

        if (memberData->gameState->selectedSquadId == -1) return;

        Squad* squad = &memberData->gameState->squads.data[memberData->gameState->selectedSquadId];

        switch (memberData->type)
        {
            case EmployeeType::ASSAULT:
            {
                Assault* assault = &memberData->gameState->assaults.data[memberData->id];

                SquadMemberAdd(squad, assault);

                break;
            }
            case EmployeeType::SCIENTIST:
            {
                Scientist* scientist = &memberData->gameState->scientists.data[memberData->id];

                SquadMemberAdd(squad, scientist);

                break;
            }
            case EmployeeType::RECON:
            {
                Recon* recon = &memberData->gameState->recons.data[memberData->id];

                SquadMemberAdd(squad, recon);

                break;
            }
            case EmployeeType::SUPPORT:
            {
                Support* support = &memberData->gameState->supports.data[memberData->id];

                SquadMemberAdd(squad, support);

                break;
            }
        }

        //Agent* agent = &memberData->gameState->agentsHired.list[memberData->agentId];

        //// TODO: Make generic and move to global
        //AddAgent(&squad->agents, *agent);

        //squad->power += agent->power;

        //Agent hired = memberData->gameState->agentsHired.list[memberData->agentId];
        //Agent last =
        //  memberData->gameState->agentsHired.list[memberData->gameState->agentsHired.count - 1];

        //memberData->gameState->agentsHired.list[memberData->agentId] = last;
        //memberData->gameState->agentsHired.count--;
    }
}

static void Property(const char* label, DF::String value)
{
    CLAY_AUTO_ID({
                .layout = {
                    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(0) },
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                },
                })
    {
        Clay_String cLabel = {
            .length = (int32_t)strlen(label),
            .chars  = label,
        };

        CLAY_TEXT(cLabel,
                  CLAY_TEXT_CONFIG({
                    .textColor = { 255, 255, 255, 255 },
                    .fontId    = 0,
                    .fontSize  = 12,
                  }));

        CLAY_AUTO_ID({ .layout = { .sizing = { .width  = CLAY_SIZING_GROW(0),
                                               .height = CLAY_SIZING_GROW(0) } } })
        {
        }

        Clay_String cValue = {
            .length = (int32_t)value.length,
            .chars  = value.data,
        };

        CLAY_TEXT(cValue,
                  CLAY_TEXT_CONFIG({
                    .textColor = { 255, 255, 255, 255 },
                    .fontId    = 0,
                    .fontSize  = 12,
                  }));
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
        CLAY(CLAY_ID("squads_list"),
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
                        .length = (int32_t)strlen(gameState->squads.data[i].name),
                        .chars  = gameState->squads.data[i].name,
                    };
                    // DF::String powerFmt =
                    //   DF::StrFormat(&gameMemory->transient, "%.2f",
                    //   gameState->squads.list[i].power);
                    // Clay_String power = {
                    //     .length = (int32_t)powerFmt.length,
                    //     .chars  = powerFmt.data,
                    // };

                    CLAY_TEXT(name,
                              CLAY_TEXT_CONFIG({
                                .textColor = { 255, 255, 255, 255 },
                                .fontId    = 0,
                                .fontSize  = 16,
                              }));

                    // CLAY_TEXT(power,
                    //           CLAY_TEXT_CONFIG({
                    //             .textColor = { 255, 255, 255, 255 },
                    //             .fontId    = 0,
                    //             .fontSize  = 16,
                    //           }));
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

        CLAY(CLAY_ID("squad_info"),
            {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                        .padding = CLAY_PADDING_ALL(16),
                        .childGap = 6,
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    },
                    .backgroundColor = { 43, 1, 151, 255 },
            })
        {
            Squad* squad = &gameState->squads.data[gameState->selectedSquadId];

            Clay_String name = {
                .length = (int32_t)strlen(squad->name),
                .chars  = squad->name,
            };

            CLAY_TEXT(name,
                      CLAY_TEXT_CONFIG({
                        .textColor = { 255, 255, 255, 255 },
                        .fontId    = 0,
                        .fontSize  = 16,
                      }));

            Property("COORDINATION",
                     DF::StrFormat(&gameMemory->transient, "%.2f", squad->coordinaton));
            Property("SUPPRESSON",
                     DF::StrFormat(&gameMemory->transient, "%.2f", squad->suppression));
            Property("PHYSICS", DF::StrFormat(&gameMemory->transient, "%.2f", squad->physics));
            Property("MECHANICS", DF::StrFormat(&gameMemory->transient, "%.2f", squad->mechanics));
            Property("BIOLOGY", DF::StrFormat(&gameMemory->transient, "%.2f", squad->biology));
            Property("CHEMISTRY", DF::StrFormat(&gameMemory->transient, "%.2f", squad->chemistry));
            Property("ELECTRONICS",
                     DF::StrFormat(&gameMemory->transient, "%.2f", squad->electronics));
            Property("DETECTION", DF::StrFormat(&gameMemory->transient, "%.2f", squad->detection));
            Property("STEALTH", DF::StrFormat(&gameMemory->transient, "%.2f", squad->stealth));
            Property("MEDICAL", DF::StrFormat(&gameMemory->transient, "%.2f", squad->medical));
            Property("LOGISTICS", DF::StrFormat(&gameMemory->transient, "%.2f", squad->logistics));
            Property("SECRECY", DF::StrFormat(&gameMemory->transient, "%.2f", squad->secrecy));

            CLAY(CLAY_ID("squad_members"),
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

                    DF::String membersNumFmt = DF::StrFormat(
                      &gameMemory->transient, "%i/%i", squad->membersCurrent, squad->membersMax);
                    Clay_String membersNum = {
                        .length = (int32_t)membersNumFmt.length,
                        .chars  = membersNumFmt.data,
                    };

                    CLAY_TEXT(membersNum,
                              CLAY_TEXT_CONFIG({
                                .textColor = { 255, 255, 255, 255 },
                                .fontId    = 0,
                                .fontSize  = 14,
                              }));
                }

                // for (int i = 0; i < squad->agents.count; ++i)
                //{
                //     CLAY_AUTO_ID(
                //{
                //     .layout = {
                //         .sizing  = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(0)
                //         }, .padding = { 16, 16, 8, 8 }, .childGap        = 16, .childAlignment  =
                //         { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER }, .layoutDirection =
                //         CLAY_LEFT_TO_RIGHT,
                //     },
                //     .backgroundColor = { 120, 120, 120, 256 },
                // })
                //     {
                //         Clay_String name = {
                //             .length = (int32_t)strlen(squad->agents.list[i].name),
                //             .chars  = squad->agents.list[i].name,
                //         };
                //         DF::String powerFmt = DF::StrFormat(
                //           &gameMemory->transient, "%.2f", squad->agents.list[i].power);
                //         Clay_String power = {
                //             .length = (int32_t)powerFmt.length,
                //             .chars  = powerFmt.data,
                //         };

                //        CLAY_TEXT(name,
                //                  CLAY_TEXT_CONFIG({
                //                    .textColor = { 255, 255, 255, 255 },
                //                    .fontId    = 0,
                //                    .fontSize  = 16,
                //                  }));

                //        CLAY_TEXT(power,
                //                  CLAY_TEXT_CONFIG({
                //                    .textColor = { 255, 255, 255, 255 },
                //                    .fontId    = 0,
                //                    .fontSize  = 16,
                //                  }));

                //        CLAY_AUTO_ID({ .layout = { .sizing = { .width  = CLAY_SIZING_GROW(0),
                //                                               .height = CLAY_SIZING_GROW(0) } }
                //                                               })
                //        {
                //        }

                //        CLAY_AUTO_ID({
                //      .layout          = {
                //            .sizing = {
                //                .width  = CLAY_SIZING_FIXED(34),
                //                .height = CLAY_SIZING_FIXED(34),
                //            },
                //            .childAlignment  = { .x = CLAY_ALIGN_X_CENTER, .y =
                //            CLAY_ALIGN_Y_CENTER },
                //      },
                //      .backgroundColor = { 64.0f, (Clay_Hovered() ? 140.0f : 128.0f), 32.0f, 1.0f
                //      }, .cornerRadius    = CLAY_CORNER_RADIUS(5),
                //    })
                //        {
                //            /*AgentData* data =
                //              (AgentData*)DF::ArenaPush(&gameMemory->transient,
                //              sizeof(AgentData));

                //            data->agentIndex = i;
                //            data->gameState  = gameState;

                //            Clay_OnHover(FireAgent, data);

                //            CLAY_TEXT(CLAY_STRING("X"),
                //                      CLAY_TEXT_CONFIG({
                //                        .textColor = { 255, 255, 255, 255 },
                //                        .fontId    = 0,
                //                        .fontSize  = 16,
                //                      }));*/
                //        }
                //    }
                //}
            }
        }

        CLAY(CLAY_ID("available_agents_list"),
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

            for (int i = 0; i < gameState->assaults.count; ++i)
            {
                Assault* assault = &gameState->assaults.data[i];

                MemberData* data =
                  (MemberData*)DF::ArenaPush(&gameMemory->transient, sizeof(MemberData));
                data->id        = assault->id;
                data->gameState = gameState;
                data->type      = EmployeeType::ASSAULT;

                AssaultListItem(gameMemory, assault, AssignMember, data);
            }

            for (int i = 0; i < gameState->scientists.count; ++i)
            {
                Scientist* scientist = &gameState->scientists.data[i];

                MemberData* data =
                  (MemberData*)DF::ArenaPush(&gameMemory->transient, sizeof(MemberData));
                data->id        = scientist->id;
                data->gameState = gameState;
                data->type      = EmployeeType::SCIENTIST;

                ScientistListItem(gameMemory, scientist, AssignMember, data);
            }

            for (int i = 0; i < gameState->recons.count; ++i)
            {
                Recon* recon = &gameState->recons.data[i];

                MemberData* data =
                  (MemberData*)DF::ArenaPush(&gameMemory->transient, sizeof(MemberData));
                data->id        = recon->id;
                data->gameState = gameState;
                data->type      = EmployeeType::RECON;

                ReconListItem(gameMemory, recon, AssignMember, data);
            }

            for (int i = 0; i < gameState->supports.count; ++i)
            {
                Support* support = &gameState->supports.data[i];

                MemberData* data =
                  (MemberData*)DF::ArenaPush(&gameMemory->transient, sizeof(MemberData));
                data->id        = support->id;
                data->gameState = gameState;
                data->type      = EmployeeType::SUPPORT;

                SupportListItem(gameMemory, support, AssignMember, data);
            }
        }
    }
}

} // namespace SCPX
