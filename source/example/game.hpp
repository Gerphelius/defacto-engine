#pragma once

#include <cstdint>

struct GameMemory
{
    bool initialized{};

    uint64_t permanentStorageSize{};
    void* permanentStorage{};

    uint64_t transientStorageSize{};
    void* transientStorage{};
};

struct GameState
{

};
