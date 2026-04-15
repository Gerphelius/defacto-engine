#pragma once

#include <cstdint>

namespace DF::Platform
{

using FileWriteTime = uint64_t;
using ProcAddress   = void*;
using Handle        = void*;

struct ModuleFilename
{
    const char* path;
    const char* filename;
};

struct DynamicLibrary
{
    Handle handle;
    FileWriteTime lastWriteTime;
};

DF_API void GetModuleFilename(char* buffer);
DF_API FileWriteTime GetFileWriteTime(const char* filename);
DF_API bool CopyFile(const char* from, const char* to, bool failIfExists);

DF_API DynamicLibrary LoadDynamicLibrary(const char* path);
DF_API bool UnloadDynamicLibrary(const DynamicLibrary* library);
DF_API ProcAddress GetFuncAddress(const DynamicLibrary* lib, const char* procName);

DF_API void* AllocateMemory(size_t size);

} // namespace DF::Platform
