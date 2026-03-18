#pragma once

#include <cstdint>
#include <filesystem>

namespace DF::PLATFORM
{

using FileWriteTime = uint64_t;
using ProcAddress   = void*;
using Handle        = void*;

struct DynamicLibrary
{
    Handle handle;
    FileWriteTime lastWriteTime;
};

std::filesystem::path GetModuleFilename();
FileWriteTime GetFileWriteTime(const char* filename);
bool CopyFile(const char* from, const char* to, bool failIfExists);

DynamicLibrary LoadDynamicLibrary(const char* path);
bool UnloadDynamicLibrary(const DynamicLibrary* library);
ProcAddress GetProcAddress(const DynamicLibrary* lib, const char* procName);

} // namespace DF::PLATFORM
