#pragma once

namespace SCP
{

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

struct SCP
{
    char name[32];
    bool captured;
    int power;
};

struct Agent
{
    char name[32];
    bool hired; // TODO: Probably don't need it, because of separate hired array
    int power;
    int squadId = -1;
};

struct Agents
{
    Agent* list;
    int count;
    int maxCount;
};

struct Squad
{
    int id;
    char name[32]; // TODO: Change to String
    int power;
    Agents agents;
};

struct Squads
{
    Squad* list;
    int count;
    int maxCount;
    int maxAgentsPerSquad;
};

enum class Fonts
{
    ROBOTO,

    TOTAL,
};

enum class MenuTab
{
    PERSONNEL,
    SQUADS,
};

struct GameState
{
    // Assets
    DF::Arena fontsArena;
    DF::Font* fonts[(int)Fonts::TOTAL];

    // Entities
    Agents agentsForHire;
    Agents agentsHired;
    Squads squads;

    // UI
    MenuTab menuTab;
    int32_t selectedSquadId;

    // 3rd party
    DF::Arena clayArena;
};

} // namespace SCP
