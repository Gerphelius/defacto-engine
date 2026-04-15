#include <Windows.h>
#include <iostream>

#undef CreateWindow
#undef UpdateWindow
#undef CopyFile
#undef DrawText
#undef GetCurrentTime
#undef GetCursorPos

#include "platform_os.hpp"

namespace DF::Platform
{

DF_API void GetModuleFilename(char* buffer)
{
    char exePath[MAX_PATH] {};
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);

    for (char* path = exePath; *path; ++path)
    {
        *buffer++ = *path;
    }

    *buffer = '\0';
}

DF_API FileWriteTime GetFileWriteTime(const char* filename)
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

DF_API bool CopyFile(const char* from, const char* to, bool failIfExists)
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

DF_API DynamicLibrary LoadDynamicLibrary(const char* path)
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

DF_API bool UnloadDynamicLibrary(const DynamicLibrary* library)
{
    if (!library->handle)
    {
        std::cerr << "Invalid library handle" << '\n';

        return false;
    }

    return FreeLibrary((HMODULE)library->handle);
}

DF_API ProcAddress GetFuncAddress(const DynamicLibrary* lib, const char* procName)
{
    return (ProcAddress)::GetProcAddress((HMODULE)lib->handle, procName);
}

DF_API void* AllocateMemory(size_t size)
{
    return VirtualAlloc(0, (size_t)size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

} // namespace DF::Platform
