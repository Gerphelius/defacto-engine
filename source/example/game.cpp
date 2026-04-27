#include <cstdio>
#include <cstring>

#include "defacto_api.hpp"
#include "scp_api.hpp"
#include "clay.h"

#include "ui/agents.cpp"
#include "ui/squads.cpp"
#include "ui/menu.cpp"
#include "ui/clay.cpp"

/*
    Entities:
        - Agents;
        - SCP;
        - Scientists;
        - Class D;

    SCPs:
        - Create a list of randomly generated SCPs;
        - Periodically add new SCP to the list for capture;

    Agents:
        - Create a list of randomly generated agents;
        - Periodically add new agent to the list of available agents to hire;
        - Hired agents go to the hired agents list;
        - Create a squad to capture SCPs
*/

DF_EXPORT_C GAME_RELOAD(GameReload)
{
    SCP::GameState* gameState = (SCP::GameState*)gameMemory->permanent.base;

    SCP::InitClay(gameState);
}

DF_EXPORT_C GAME_INITIALIZE(GameInitialize)
{
    DF::GameMemory gameMemory {};
    gameMemory.permanent = DF::Platform::AllocateMemory(Megabytes(64));
    gameMemory.transient = DF::Platform::AllocateMemory(Gigabytes(1));

    SCP::GameState* gameState =
      (SCP::GameState*)DF::ArenaPush(&gameMemory.permanent, sizeof(SCP::GameState));

    int totalFontSize = Kilobytes(256);

    gameState->fontsArena.base = DF::ArenaPush(&gameMemory.permanent, totalFontSize);
    gameState->fontsArena.size = totalFontSize;

    DF::Font* roboto = DF::LoadFont("resources/fonts/roboto_regular.json",
                                    "resources/fonts/roboto_regular.png",
                                    &gameState->fontsArena);

    if (roboto)
    {
        gameState->fonts[(int)SCP::Fonts::ROBOTO] = roboto;
    }

    int clayMemorySize        = Clay_MinMemorySize();
    gameState->clayArena.base = DF::ArenaPush(&gameMemory.permanent, Clay_MinMemorySize());
    gameState->clayArena.size = clayMemorySize;
    SCP::InitClay(gameState);

    gameState->agentsForHire.maxCount = 10;
    gameState->agentsForHire.list     = (SCP::Agent*)DF::ArenaPush(
      &gameMemory.permanent, gameState->agentsForHire.maxCount * sizeof(SCP::Agent));
    gameState->agentsHired.maxCount = 20;
    gameState->agentsHired.list     = (SCP::Agent*)DF::ArenaPush(
      &gameMemory.permanent, gameState->agentsHired.maxCount * sizeof(SCP::Agent));

    /*
       [ Squad[A,A,A,A...], Squad[A,A,A,A...], Squad[A,A,A,A...], Squad[A,A,A,A...] ]
    
    */

    gameState->squads.maxCount          = 4;
    gameState->squads.maxAgentsPerSquad = 8;
    gameState->squads.list              = (SCP::Squad*)DF::ArenaPush(
      &gameMemory.permanent,
      gameState->squads.maxCount *
        (sizeof(SCP::Squad) + sizeof(SCP::Agent) * gameState->squads.maxAgentsPerSquad));

    for (int i = 0; i < 10; ++i)
    {
        SCP::AddAgent(&gameState->agentsForHire, SCP::GenerateRandomAgent());
    }

    return gameMemory;
}

static float timePassed = 0.0f;

DF_EXPORT_C GAME_UPDATE(GameUpdate)
{
    // SCP::GameState* gameState = (SCP::GameState*)gameMemory->permanent.base;

    // if (timePassed > 1.0f)
    //{
    //     SCP::AddAgent(&gameState->agentsForHire, SCP::GenerateRandomAgent());
    //     timePassed = 0.0f;
    // }

    // timePassed += dt;

    SCP::RenderUI(gameMemory, dt);
}

////**************************** ENGINE PART ****************************//
//
// struct Arena
//{
//    void* base;
//    int offset;
//    int sizeTotal;
//    int sizeLeft;
//};
//
// void* ArenaPush(Arena* arena, int size)
//{
//    if (size > arena->sizeLeft)
//    {
//        // Error
//    }
//
//    void* res = (char*)arena->base + arena->offset;
//
//    arena->offset += size;
//    arena->sizeLeft -= size;
//
//    return res;
//}
//
// struct GameMemory
//{
//    Arena permanent;
//    Arena transient;
//};
//
// struct Font
//{
//    struct Glyph
//    {
//    };
//    int glyphCount;
//    Glyph* glyphs;
//};
//
// Font* LoadFont(const char* bitmap, const char* data, Arena* arena)
//{
//    // Load bitmap
//    // Load data
//
//    int fontSize = sizeof(Font) + sizeof(Glyph) * number_of_glyphs;
//    Font* font   = ArenaPush(arena, fontSize);
//
//    // Returns valid Font pointer or nullptr if failed
//    return font;
//}
//
////**************************** GAME PART ****************************//
//
// enum class Fonts
//{
//    ROBOTO,
//    SANS,
//    BITCOUNT,
//
//    TOTAL,
//};
//
// struct GameState
//{
//    Arena fontsArena;
//    Font* fonts[(int)Fonts::TOTAL];
//
//    Arena entities;
//    // ...
//};
//
// GameMemory GameInit()
//{
//    GameMemory gameMemory {};
//    gameMemory.permanent = AllocateMemory(size);
//
//    GameState* gameState = ArenaPush(&gameMemory.permanent, sizeof(GameState));
//
//    int totalFontSize = offset_from_base_to_place_first_font_data;
//
//    gameState->fontsArena.base      = ArenaPush(&gameMemory.permanent, totalFontSize);
//    gameState->fontsArena.offset    = 0;
//    gameState->fontsArena.sizeTotal = totalFontSize;
//    gameState->fontsArena.sizeLeft  = totalFontSize;
//
//    Font* roboto = LoadFont("roboto_bitmap", "roboto_data", &gameState->fontsArena);
//
//    if (roboto)
//    {
//        gameState->fonts[(int)Fonts::ROBOTO] = roboto;
//    }
//
//    // Usage example
//    int count = gameState->fonts[(int)Fonts::ROBOTO]->glyphCount;
//
//    // This returns gameMemory struct, so it will be saved inside main.exe
//    // and passed to the game on update.
//    return gameMemory;
//}
