#include <cassert>

#include "game.hpp"

#include "defacto_api.hpp"

DF_EXPORT ON_UPDATE(OnUpdate)
{
    // assert(sizeof(GameState) <= memory->permanentStorageSize);

    // GameState* gameState = static_cast<GameState*>(memory->permanentStorage);

    return "Hot Reload 11221";
}
