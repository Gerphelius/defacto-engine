#pragma once

#include <cstdint>
#include <filesystem>

namespace DF::PLATFORM
{
    using FileWriteTime = uint64_t;
    using ProcAddress = void*;
    using Handle = void*;

    struct DynamicLibrary
    {
        Handle handle;
        FileWriteTime lastWriteTime;
    };

    #define WINDOW_CREATE(name) void* name(int width, int height, const char* title)
    WINDOW_CREATE(CreateWindow);

    #define WINDOW_UPDATE(name) void name(void* window)
    WINDOW_UPDATE(UpdateWindow);

    #define WINDOW_CLOSED(name) bool name(void* window)
    WINDOW_CLOSED(WindowClosed);

    #define GET_FILE_WRITE_TIME(name) FileWriteTime name(const char* filename)
    GET_FILE_WRITE_TIME(GetFileWriteTime);

    #define LOAD_DYNAMIC_LIBRARY(name) DynamicLibrary name(const char* path)
    LOAD_DYNAMIC_LIBRARY(LoadDynamicLibrary);

    #define UNLOAD_DYNAMIC_LIBRARY(name) bool name(const DynamicLibrary* library)
    UNLOAD_DYNAMIC_LIBRARY(UnloadDynamicLibrary);

    #define GET_PROC_ADDRESS(name) ProcAddress name(const DynamicLibrary* lib, const char* procName)
    GET_PROC_ADDRESS(GetProcAddress);

    #define COPY_FILE(name) bool name(const char* from, const char* to, bool failIfExists)
    COPY_FILE(CopyFile);

    #define GET_MODULE_FILENAME(name) std::filesystem::path name()
    GET_MODULE_FILENAME(GetModuleFilename);
}
