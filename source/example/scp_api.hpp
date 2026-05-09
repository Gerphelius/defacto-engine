#pragma once

namespace SCPX
{

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

template<typename T>
struct List
{
    T* data;
    int count;
    int maxCount;
};

template <typename T>
inline void ListAdd(List<T>* list, T value)
{
    if (list->count < list->maxCount)
    {
        list->data[list->count++] = value;
    }
}

struct SCP
{
    enum class ContainmentClass
    {
        UNKNOWN,
        SAFE,
        EUCLID,
        KETER,
    };
    enum class ContainmentMode
    {
        UNCONTAINABLE,
        FACILITY,
        IN_PLACE,
    };
    enum class Type
    {
        UNKNOWN,
        HUMANOID,
        MECHANICAL,
        BIOLOGICAL,
        CHEMICAL,
        STRUCTURE,
        ITEM,
    };
    enum class Cognition
    {
        UNKNOWN,
        NONE,
        ANIMALISTIC,
        HUMAN,
        STATEGIC,
        SUPERHUMAN,
    };
    enum class Movement
    {
        UNKNOWN,
        NONE,
        GROUND,
        WATER,
        AIR,
        TELEPORT,
        TRANSDIMENSIONAL,
        BODY,
    };
    enum class Mobility
    {
        UNKNOWN,
        STATIC,
        SLOW,
        FAST,
        EXTREME,
        INSTANT,
    };
    enum class DamageType
    {
        UNKNOWN,
        NONE,
        PHYSICAL,
        CHEMICAL,
        BIOLOGICAL,
        ELEMENTAL,
        MENTAL,
    };
    enum class Behavior
    {
        UNKNOWN,
        DORMANT,
        STABLE,
        AGITATED,
        HOSTILE,
        BREACH,
    };

    template<typename T>
    struct Property
    {
        T value;
        float difficulty;
        float progress;
        bool revealed;
    };

    int id = -1;
    Property<char[32]> name;
    Property<ContainmentClass> containmentClass;
    Property<ContainmentMode> containmentMode;
    Property<Type> type;
    Property<Cognition> cognition;
    Property<Movement> movement;
    Property<Mobility> mobility;
    Property<DamageType> damageType;
    Property<float> damage;
    Property<float> threat;
    Property<float> squadCoordination;
    Property<uint32_t> squadSize;
    Property<float> evasion;
    Property<Behavior> behaviorState;
};

enum class EmployeeType
{
    ASSAULT,
    SCIENTIST,
    RECON,
    SUPPORT,
};

struct Assault
{
    int id = -1;
    char name[32];
    int squadId = -1;
    float suppression;
};

struct Scientist
{
    int id = -1;
    char name[32];
    int squadId = -1;
    float mechanics;
    float physics;
    float biology;
    float chemistry;
    float electronics;
};

struct Recon
{
    int id = -1;
    char name[32];
    int squadId = -1;
    float detection;
    float stealth;
};

struct Support
{
    int id = -1;
    char name[32];
    int squadId = -1;
    float medical;
    float logistics;
    float secrecy;
};

struct Squad
{
    int id = -1;
    char name[32];
    int missionId;

    // TODO: Change back to actual structs instead of indices

    List<int> assaultIds;
    List<int> scientistIds;
    List<int> reconIds;
    List<int> supportIds;
    uint32_t membersCurrent;
    uint32_t membersMax;

    float coordinaton;
    float suppression;
    float mechanics;
    float physics;
    float biology;
    float chemistry;
    float electronics;
    float detection;
    float stealth;
    float medical;
    float logistics;
    float secrecy;
};

struct Mission
{
    int id = -1;
    char name[32];
    int squadId = -1;
    int scpId = -1;

    struct Stage
    {
        enum Type
        {
            PENDING,
            SURVEILLANCE,
            SECURING,
        };

        Type type;
        float progress;
    };
    Stage stage;
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
    DF::Font* fonts[(int)Fonts::TOTAL]; // TODO: Change to List

    // Entities
    List<Assault> assaults;
    List<Scientist> scientists;
    List<Recon> recons;
    List<Support> supports;
    List<SCP> scps;
    List<Mission> missions;
    List<Squad> squads;
    uint32_t maxMembersPerSquad;

    // Globals
    float worldStabilty;

    // UI
    MenuTab menuTab;
    int selectedSquadId;
    int selectedMissionId;
    bool showMissionSquadAssign;

    // 3rd party
    DF::Arena clayArena;
};

} // namespace SCPX
