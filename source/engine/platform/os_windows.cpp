#include <Windows.h>
#include <iostream>

#undef CreateWindow
#undef UpdateWindow
#undef CopyFile
#undef DrawText

#include "platform_os.hpp"

namespace DF::Platform
{

std::filesystem::path GetModuleFilename()
{
    char exePath[MAX_PATH] {};
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);

    return exePath;
}

FileWriteTime GetFileWriteTime(const char* filename)
{
    FileWriteTime lastWriteTime {};
    WIN32_FIND_DATA findData {};
    HANDLE findHandle = FindFirstFileA(filename, &findData);

    if (findHandle != INVALID_HANDLE_VALUE)
    {
        lastWriteTime =
          findData.ftLastWriteTime.dwLowDateTime + findData.ftLastWriteTime.dwHighDateTime;
        FindClose(findHandle);
    }

    return lastWriteTime;
}

bool CopyFile(const char* from, const char* to, bool failIfExists)
{
    bool success = true;

    if (!::CopyFileA(from, to, failIfExists))
    {
        DWORD error = GetLastError();

        std::cerr << "CopyFile failed: " << error << '\n';

        success = false;
    }

    return success;
}

DynamicLibrary LoadDynamicLibrary(const char* path)
{
    DynamicLibrary library {};

    library.handle        = LoadLibraryA(path);
    library.lastWriteTime = GetFileWriteTime(path);

    if (!library.handle)
    {
        DWORD error = GetLastError();

        std::cerr << "LoadDynamicLibrary failed: " << error << '\n';
    }

    return library;
}

bool UnloadDynamicLibrary(const DynamicLibrary* library)
{
    if (!library->handle)
    {
        std::cerr << "Invalid library handle" << '\n';

        return false;
    }

    return FreeLibrary((HMODULE)library->handle);
}

ProcAddress GetProcAddress(const DynamicLibrary* lib, const char* procName)
{
    return (ProcAddress)::GetProcAddress((HMODULE)lib->handle, procName);
}

} // namespace DF::Platform
