#pragma once

namespace SCPX
{

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

struct SCP
{
    enum Containment
    {
        UNKNOWN,
        SAFE,
        EUCLID,
        KETER,
    };

    char name[32];
    bool captured;
    bool movable;
    float power;
    Containment containment;
};

struct SCPs
{
    SCP* list;
    int count;
    int maxCount;
};

struct Agent
{
    char name[32];
    bool hired; // TODO: Probably don't need it, because of separate hired array
    float power;
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
    char name[32];
    float power;
    int missionId;
    Agents agents;
};

struct Squads
{
    Squad* list;
    int count;
    int maxCount;
    int maxAgentsPerSquad;
};

struct Mission
{
    int id;
    char name[32];
    int squadId;
    int scpId;
    float difficultyFactor;

    struct Stage
    {
        enum Type
        {
            INVESTIGATION,
            SURVEILLANCE,
            CONTAINMENT,
        };

        Type type;
        float progress;
    };
    Stage stage;
};

struct Missions
{
    Mission* list;
    int count;
    int maxCount;
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
    MISSIONS,
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
    SCPs scps;
    Missions missions;

    // UI
    MenuTab menuTab;
    int selectedSquadId;
    int selectedMissionId;
    bool showMissionSquadAssign;

    // 3rd party
    DF::Arena clayArena;
};

} // namespace SCPX
