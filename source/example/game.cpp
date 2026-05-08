#include <cstdio>
#include <cstring>

#include "defacto_api.hpp"
#include "scp_api.hpp"
#include "clay.h"

// #include "ui/agents.cpp"
// #include "ui/squads.cpp"
// #include "ui/missions.cpp"
// #include "ui/menu.cpp"
#include "ui/clay.cpp"

using namespace SCPX;

static void CreateMission(GameState* gameState)
{
    // SCP* scp         = &gameState->scps.list[gameState->missions.count];
    // Mission* mission = &gameState->missions.list[gameState->missions.count];

    // mission->scpId = gameState->missions.count;
    // mission->id    = gameState->missions.count++;
    // mission->squadId = -1;
    // DF::StrFormat(mission->name, sizeof(mission->name), "M-%i", gameState->missions.count);
    // mission->difficultyFactor = 10.0f * scp->power * (float)scp->containment;
    // mission->stage.type       = Mission::Stage::INVESTIGATION;
    // mission->stage.progress   = 0.0f;
}

DF_EXPORT_C GAME_RELOAD(GameReload)
{
    GameState* gameState = (GameState*)gameMemory->permanent.base;

    InitClay(gameState);
}

DF_EXPORT_C GAME_INITIALIZE(GameInitialize)
{
    DF::GameMemory gameMemory {};
    gameMemory.permanent = DF::Platform::AllocateMemory(MEGABYTES(64));
    gameMemory.transient = DF::Platform::AllocateMemory(GIGABYTES(1));

    GameState* gameState = (GameState*)DF::ArenaPush(&gameMemory.permanent, sizeof(GameState));

    int totalFontSize = KILOBYTES(256);

    gameState->fontsArena.base = DF::ArenaPush(&gameMemory.permanent, totalFontSize);
    gameState->fontsArena.size = totalFontSize;

    DF::Font* roboto = DF::LoadFont("resources/fonts/roboto_regular.json",
                                    "resources/fonts/roboto_regular.png",
                                    &gameState->fontsArena);

    if (roboto)
    {
        gameState->fonts[(int)Fonts::ROBOTO] = roboto;
    }

    int clayMemorySize        = Clay_MinMemorySize();
    gameState->clayArena.base = DF::ArenaPush(&gameMemory.permanent, Clay_MinMemorySize());
    gameState->clayArena.size = clayMemorySize;
    InitClay(gameState);

    gameState->maxMembersPerSquad = 4;
    gameState->squads.maxCount    = 4;
    gameState->squads.data =
      (Squad*)DF::ArenaPush(&gameMemory.permanent,
                            gameState->squads.maxCount *
                              (sizeof(Squad) + sizeof(Scientist) * gameState->maxMembersPerSquad));

    // TODO:
    // Create agents and squad
    // Assign agents to squad
    // Assign squad to the mission
    // Create mission ui with updated scp data
    // Player can start/stop mission state

    gameState->scps.maxCount = 4;
    gameState->scps.data     = (SCP*)DF::ArenaPush(
      &gameMemory.permanent, gameState->scps.maxCount * (sizeof(SCP) * gameState->scps.maxCount));

    gameState->scps.data[0] = {
        .name              = { "SCP-173", 10.0f },
        .containmentClass  = { SCP::ContainmentClass::EUCLID, 30.0f },
        .containmentMode   = { SCP::ContainmentMode::FACILITY, 30.0f },
        .type              = { SCP::Type::HUMANOID, 10.0f },
        .cognition         = { SCP::Cognition::HUMAN, 80.0f },
        .movement          = { SCP::Movement::GROUND, 50.0f },
        .mobility          = { SCP::Mobility::EXTREME, 10.0f },
        .damageType        = { SCP::DamageType::PHYSICAL, 20.0f },
        .damage            = { 100.0f, 20.0f },
        .threat            = { 60.0f, 10.0f },
        .squadCoordination = { 70.0f, 50.0f },
        .squadSize         = { 3, 30.0f },
        .evasion           = { 40.0f, 80.0f },
        .behaviorState     = { SCP::Behavior::HOSTILE, 40.0f },
    };

    gameState->missions.maxCount = 10;
    gameState->missions.data     = (Mission*)DF::ArenaPush(
      &gameMemory.permanent,
      gameState->missions.maxCount * (sizeof(Mission) * gameState->missions.maxCount));

    gameState->missions.data[0] = {
        .name    = "M-001",
        .squadId = -1,
        .scpId   = 0,
        .stage   = Mission::Stage::PENDING,
    };

    return gameMemory;
}

DF_EXPORT_C GAME_UPDATE(GameUpdate)
{
    GameState* gameState = (GameState*)gameMemory->permanent.base;

    // for (int i = 0; i < gameState->missions.count; ++i)
    //{
    //     Mission* mission = &gameState->missions.list[i];

    //    if (mission->squadId < 0) continue;

    //    Squad* missionSquad = &gameState->squads.list[mission->squadId];

    //    if (missionSquad->power <= 0.0f) continue;

    //    if (mission->stage.progress < 100.0f)
    //    {
    //        mission->stage.progress += dt * 10;
    //    }
    //}

    RenderUI(gameMemory, dt);
}
