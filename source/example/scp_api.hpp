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
    bool hired;   // TODO: Probably don't need it, because of separate hired array
    int power;
    int squadId = -1;
};

struct Squad
{
    char name[32];
    int power;
    Agent* agents;
};

struct Agents
{
    Agent* list;
    int count;
    int maxCount;
};

enum class Fonts
{
    ROBOTO,

    TOTAL,
};

struct GameState
{
    DF::Arena fontsArena;
    DF::Assets::Font* fonts[(int)Fonts::TOTAL];

    DF::Arena clayArena;

    Agents agentsForHire;
    Agents agentsHired;

    // DF::Arena entities;
    // ...
};

} // namespace SCP
