#include <chrono>
#include <thread>
#include <filesystem>

#include "defacto_api.hpp"
#include "platform/platform_os.hpp"
#include "platform/platform_window.hpp"

inline ON_UPDATE(OnUpdateStub)
{
    return "Stub";
}

using namespace DF;

struct GameCode
{
    Platform::DynamicLibrary library;
    API::OnUpdateT* OnUpdate = OnUpdateStub;
};

static GameCode LoadGameCode(const char* dllPath, const char* tempDllPath)
{
    // Used to give os some time to finish building new game.dll so it won't be locked
    // and be able to create a copy of it for hot-reload
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    GameCode gameCode {};

    if (!Platform::CopyFile(dllPath, tempDllPath, false))
        return gameCode;

    gameCode.library = Platform::LoadDynamicLibrary(tempDllPath);

    if (!gameCode.library.handle)
        return gameCode;

    gameCode.OnUpdate = (API::OnUpdateT*)Platform::GetProcAddress(&gameCode.library, "OnUpdate");

    return gameCode;
}

static void UnloadGameCode(GameCode* code)
{
    if (code->library.handle)
    {
        Platform::UnloadDynamicLibrary(&code->library);
    }
}

/*  *********************************  OVERVIEW  *********************************
    -- Project is divided into parts where:
        * game related code (dll) which uses engine modules if needed.
        * engine entry point (exe) which loads game code at runtime,
        initializes modules/memory and run game loop.

    -- Project uses unity build, so there will be a smol number of compilation units,
    but at least 1 per module listed above.

    -- Game will not allocate memory directly, but can ask for more memory from engine,
    so hot-reload won't break game state.
*/

int main()
{
    std::filesystem::path exePath = Platform::GetModuleFilename();
    std::filesystem::path dllPath = exePath.parent_path();

    std::string gameDllPath     = (dllPath / "game.dll").string();
    std::string gameDllPathTemp = (dllPath / ("game_temp.dll")).string();

    GameCode gameCode = LoadGameCode(gameDllPath.c_str(), gameDllPathTemp.c_str());

    Platform::Window window = Platform::CreateWindow(800, 600, "Test");

    Render::Initialize(&window);
    UI::Initialize();

    while (!Platform::WindowClosed(&window))
    {
        Platform::FileWriteTime lastGameDllWriteTime =
          Platform::GetFileWriteTime(gameDllPath.c_str());

        if (gameCode.library.lastWriteTime != lastGameDllWriteTime)
        {
            UnloadGameCode(&gameCode);
            gameCode = LoadGameCode(gameDllPath.c_str(), gameDllPathTemp.c_str());
        }

        std::cout << gameCode.OnUpdate() << '\n';

        Render::BeginFrame();

        Platform::Size fbSize = Platform::GetFramebufferSize(&window);
        UI::Render(fbSize.width, fbSize.height);

        Render::EndFrame();
    }

    UnloadGameCode(&gameCode);

    return 0;
}
