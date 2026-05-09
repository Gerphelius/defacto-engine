#include <cstdio>
#include <cstring>

#include "defacto_api.hpp"
#include "scp_api.hpp"
#include "clay.h"

#include "ui/shared.cpp"
#include "ui/agents.cpp"
#include "ui/squads.cpp"
#include "ui/missions.cpp"
#include "ui/menu.cpp"
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

    /// *********************************** SQUADS *********************************** ///

    int employeeTypesNum          = 4;
    gameState->maxMembersPerSquad = 4;
    gameState->squads.maxCount    = 4;
    gameState->squads.data        = (Squad*)DF::ArenaPush(
      &gameMemory.permanent,
      gameState->squads.maxCount *
        (sizeof(Squad) + sizeof(int) * employeeTypesNum * gameState->maxMembersPerSquad));

    /// *********************************** ASSAULTS *********************************** ///

    gameState->assaults.maxCount = 10;
    gameState->assaults.data     = (Assault*)DF::ArenaPush(
      &gameMemory.permanent, sizeof(Assault) * gameState->assaults.maxCount);

    for (int i = 0; i < gameState->assaults.maxCount; ++i)
    {
        static float suppression = 20.0f;

        if (i >= 6)
        {
            suppression = 30.0f;
        }
        else if (i >= 3)
        {
            suppression = 40.0f;
        }

        gameState->assaults.data[i] = {
            .id          = i,
            .suppression = suppression,
        };

        DF::StrFormat(gameState->assaults.data[i].name,
                      (int)sizeof(gameState->assaults.data[i]),
                      "Assault 00%i",
                      i);

        ++gameState->assaults.count;
    }

    /// *********************************** SCIENTISTS *********************************** ///

    gameState->scientists.maxCount = 10;
    gameState->scientists.data     = (Scientist*)DF::ArenaPush(
      &gameMemory.permanent, sizeof(Scientist) * gameState->scientists.maxCount);

    for (int i = 0; i < gameState->scientists.maxCount; ++i)
    {
        static float scalar = 1.0f;

        float mechanics   = 1.0f;
        float electronics = 0.0f;
        float biology     = 0.0f;
        float chemistry   = 0.0f;
        float physics     = 0.0f;
        float value       = 20.0f;

        if (i >= 8)
        {
            scalar      = 1.0f;
            mechanics   = 0.0f;
            electronics = 1.0f;
        }
        else if (i >= 6)
        {
            scalar      = 1.0f;
            mechanics   = 0.0f;
            electronics = 0.0f;
            biology     = 1.0f;
        }
        else if (i >= 4)
        {
            scalar      = 1.0f;
            mechanics   = 0.0f;
            electronics = 0.0f;
            biology     = 0.0f;
            chemistry   = 1.0f;
        }
        else if (i >= 2)
        {
            scalar      = 1.0f;
            mechanics   = 0.0f;
            electronics = 0.0f;
            biology     = 0.0f;
            chemistry   = 0.0f;
            physics     = 1.0f;
        }

        gameState->scientists.data[i] = {
            .id          = i,
            .mechanics   = mechanics * value * scalar,
            .physics     = physics * value * scalar,
            .biology     = biology * value * scalar,
            .chemistry   = chemistry * value * scalar,
            .electronics = electronics * value * scalar,
        };

        ++scalar;

        DF::StrFormat(gameState->scientists.data[i].name,
                      (int)sizeof(gameState->scientists.data[i]),
                      "Scientist 00%i",
                      i);

        ++gameState->scientists.count;
    }

    /// *********************************** RECONS *********************************** ///

    gameState->recons.maxCount = 10;
    gameState->recons.data =
      (Recon*)DF::ArenaPush(&gameMemory.permanent, sizeof(Recon) * gameState->recons.maxCount);

    for (int i = 0; i < gameState->recons.maxCount; ++i)
    {
        static float scalar = 1.0f;

        float detection = 1.0f;
        float stealth   = 0.0f;
        float value     = 20.0f;

        if (i >= 5)
        {
            scalar    = 1.0f;
            detection = 0.0f;
            stealth   = 1.0f;
        }

        gameState->recons.data[i] = {
            .id        = i,
            .detection = detection * value * scalar,
            .stealth   = stealth * value * scalar,
        };

        ++scalar;

        DF::StrFormat(
          gameState->recons.data[i].name, (int)sizeof(gameState->recons.data[i]), "Recon 00%i", i);

        ++gameState->recons.count;
    }

    /// *********************************** SUPPORTS *********************************** ///

    gameState->supports.maxCount = 10;
    gameState->supports.data     = (Support*)DF::ArenaPush(
      &gameMemory.permanent, sizeof(Support) * gameState->supports.maxCount);

    for (int i = 0; i < gameState->supports.maxCount; ++i)
    {
        static float scalar = 1.0f;

        float medical   = 1.0f;
        float logistics = 0.0f;
        float secrecy   = 0.0f;
        float value     = 20.0f;

        if (i >= 6)
        {
            scalar    = 1.0f;
            medical   = 0.0f;
            logistics = 1.0f;
        }
        else if (i >= 3)
        {
            scalar    = 1.0f;
            medical   = 0.0f;
            logistics = 0.0f;
            secrecy   = 1.0f;
        }

        gameState->supports.data[i] = {
            .id        = i,
            .medical   = medical * value * scalar,
            .logistics = logistics * value * scalar,
            .secrecy   = secrecy * value * scalar,
        };

        ++scalar;

        DF::StrFormat(gameState->supports.data[i].name,
                      (int)sizeof(gameState->supports.data[i]),
                      "Support 00%i",
                      i);

        ++gameState->supports.count;
    }

    // TODO:
    // Create agents and squad
    // Assign agents to squad
    // Assign squad to the mission
    // Create mission ui with updated scp data
    // Player can start/stop mission state

    gameState->scps.maxCount = 4;
    gameState->scps.data     = (SCP*)DF::ArenaPush(
      &gameMemory.permanent, gameState->scps.maxCount * (sizeof(SCP) * gameState->scps.maxCount));

    gameState->scps.data[gameState->scps.count++] = {
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
