#include <Windows.h>
#include <iostream>
#include <filesystem>

#undef CreateWindow
#undef UpdateWindow
#undef CopyFile

#include "platform.hpp"

namespace DF::PLATFORM
{
    GET_FILE_WRITE_TIME(GetFileWriteTime)
    {
        FileWriteTime lastWriteTime{};
        WIN32_FIND_DATA findData{};
        HANDLE findHandle = FindFirstFileA(filename, &findData);

        if (findHandle != INVALID_HANDLE_VALUE)
        {
            lastWriteTime = findData.ftLastWriteTime.dwLowDateTime + findData.ftLastWriteTime.dwHighDateTime;
            FindClose(findHandle);
        }

        return lastWriteTime;
    }

    LOAD_DYNAMIC_LIBRARY(LoadDynamicLibrary)
    {
        DynamicLibrary library{};

        library.handle = LoadLibraryA(path);
        library.lastWriteTime = GetFileWriteTime(path);

        if (!library.handle)
        {
            DWORD error = GetLastError();

            std::cerr << "LoadDynamicLibrary failed: " << error << '\n';

            // LOG: "Library loading failed"
        }

        return library;
    }

    UNLOAD_DYNAMIC_LIBRARY(UnloadDynamicLibrary)
    {
        if (!library->handle)
        {
            // LOG: "Invalid library handle"

            return false;
        }

        return FreeLibrary((HMODULE)library->handle);
    }

    GET_PROC_ADDRESS(GetProcAddress)
    {
        return (ProcAddress)::GetProcAddress((HMODULE)lib->handle, procName);
    }

    COPY_FILE(CopyFile)
    {
        bool success = true;

        if (!::CopyFileA(from, to, failIfExists))
        {
            DWORD error = GetLastError();

            std::cerr << "CopyFile failed: " << error << '\n';

            // LOG: "CopyFile failed"

            success = false;
        }

        return success;
    }

    GET_MODULE_FILENAME(GetModuleFilename)
    {
        char exePath[MAX_PATH]{};
        GetModuleFileNameA(nullptr, exePath, MAX_PATH);

        return exePath;
    }
}
