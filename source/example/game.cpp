#include <cstdio>
#include <cstring>

#include "defacto_api.hpp"
#include "scp_api.hpp"
#include "clay.h"

#include "ui/agents.cpp"
#include "ui/squads.cpp"
#include "ui/missions.cpp"
#include "ui/menu.cpp"
#include "ui/clay.cpp"

using namespace SCPX;

static void CreateMission(GameState* gameState)
{
    SCP* scp         = &gameState->scps.list[gameState->missions.count];
    Mission* mission = &gameState->missions.list[gameState->missions.count];

    mission->scpId = gameState->missions.count;
    mission->id    = gameState->missions.count++;
    mission->squadId = -1;
    DF::StrFormat(mission->name, sizeof(mission->name), "M-%i", gameState->missions.count);
    mission->difficultyFactor = 10.0f * scp->power * (float)scp->containment;
    mission->stage.type       = Mission::Stage::INVESTIGATION;
    mission->stage.progress   = 0.0f;
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

    gameState->agentsForHire.maxCount = 10;
    gameState->agentsForHire.list     = (Agent*)DF::ArenaPush(
      &gameMemory.permanent, gameState->agentsForHire.maxCount * sizeof(Agent));
    gameState->agentsHired.maxCount = 20;
    gameState->agentsHired.list =
      (Agent*)DF::ArenaPush(&gameMemory.permanent, gameState->agentsHired.maxCount * sizeof(Agent));

    gameState->squads.maxCount          = 4;
    gameState->squads.maxAgentsPerSquad = 8;
    gameState->squads.list              = (Squad*)DF::ArenaPush(
      &gameMemory.permanent,
      gameState->squads.maxCount *
        (sizeof(Squad) + sizeof(Agent) * gameState->squads.maxAgentsPerSquad));
    gameState->selectedSquadId   = -1;
    gameState->selectedMissionId = -1;

    gameState->missions.maxCount = 10;
    gameState->missions.list     = (Mission*)DF::ArenaPush(
      &gameMemory.permanent, gameState->missions.maxCount * sizeof(Mission));

    SCP scps[] {
        { "SCP-173", false, true, 100.0f, SCP::EUCLID },
        { "SCP-096", false, true, 150.0f, SCP::EUCLID },
        { "SCP-087", false, false, 20.0f, SCP::EUCLID },
        { "SCP-106", false, true, 400.0f, SCP::KETER },
        { "SCP-914", false, true, 0.0f, SCP::SAFE },
    };

    gameState->scps.maxCount = ArrayCount(scps);
    gameState->scps.list =
      (SCP*)DF::ArenaPush(&gameMemory.permanent, gameState->scps.maxCount * sizeof(SCP));

    for (int i = 0; i < ArrayCount(scps); ++i)
    {
        gameState->scps.list[i] = scps[i];
        ++gameState->scps.count;
    }

    for (int i = 0; i < 10; ++i)
    {
        AddAgent(&gameState->agentsForHire, GenerateRandomAgent());
    }

    for (int i = 0; i < gameState->scps.count; ++i)
    {
        CreateMission(gameState);
    }

    return gameMemory;
}

DF_EXPORT_C GAME_UPDATE(GameUpdate)
{
    GameState* gameState = (GameState*)gameMemory->permanent.base;

    for (int i = 0; i < gameState->missions.count; ++i)
    {
        Mission* mission = &gameState->missions.list[i];

        if (mission->squadId < 0) continue;

        Squad* missionSquad = &gameState->squads.list[mission->squadId];

        if (missionSquad->power <= 0.0f) continue;

        if (mission->stage.progress < 100.0f)
        {
            mission->stage.progress += dt * 10;
        }
    }

    RenderUI(gameMemory, dt);
}
