/*  *********************************  OVERVIEW  *********************************

    -- Project is divided into parts where:
        * game related code (dll) which uses engine modules if needed.
        * engine entry point (exe) which loads game code at runtime,
        initializes modules/memory and run game loop.

    -- Project uses unity build, so there will be a smol number of compilation units,
    but at least 1 per module listed above.

    -- Game will not allocate memory directly, but can ask for it from engine,
    so hot-reload won't break game state.

    UPD: Introduced 3rd module for engine code that goes to a lib so it can be linked to
         the game dll and main.exe, which contained engine lib code previously.

*  *******************************************************************************/

#include <chrono>
#include <thread>
#include <filesystem>
#include <iostream>

#include "defacto_api.hpp"

typedef GAME_INITIALIZE(GameInitializeT);
inline GAME_INITIALIZE(GameInitializeStub)
{
    return DF::GameMemory {};
}

typedef GAME_UPDATE(GameUpdateT);
inline GAME_UPDATE(GameUpdateStub)
{
}

typedef GAME_RELOAD(GameReloadT);
inline GAME_RELOAD(GameReloadStub)
{
}

using namespace DF;

struct GameCode
{
    Platform::DynamicLibrary library;
    GameInitializeT* GameIntialize = GameInitializeStub;
    GameReloadT* GameReload        = GameReloadStub;
    GameUpdateT* GameUpdate        = GameUpdateStub;
    bool valid;
};

static GameCode LoadGameCode(const char* dllPath, const char* tempDllPath)
{
    // Used to give os some time to finish building new game.dll so it won't be locked
    // and be able to create a copy of it for hot-reload
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    GameCode gameCode {};

    if (!Platform::CopyFile(dllPath, tempDllPath, false)) return gameCode;

    gameCode.library = Platform::LoadDynamicLibrary(tempDllPath);

    if (!gameCode.library.handle) return gameCode;

    gameCode.GameUpdate = (GameUpdateT*)Platform::GetFuncAddress(&gameCode.library, "GameUpdate");
    gameCode.GameReload = (GameReloadT*)Platform::GetFuncAddress(&gameCode.library, "GameReload");
    gameCode.GameIntialize =
      (GameInitializeT*)Platform::GetFuncAddress(&gameCode.library, "GameInitialize");

    if (!gameCode.GameUpdate || !gameCode.GameIntialize || !gameCode.GameReload) return gameCode;

    gameCode.valid = true;

    return gameCode;
}

static void UnloadGameCode(GameCode* code)
{
    if (code->library.handle)
    {
        Platform::UnloadDynamicLibrary(&code->library);
    }
}

static bool g_running;

int main()
{
    char buffer[260];
    Platform::GetModuleFilename(buffer);

    std::filesystem::path exePath = buffer;
    std::filesystem::path dllPath = exePath.parent_path();

    std::string gameDllPath     = (dllPath / "game.dll").string();
    std::string gameDllPathTemp = (dllPath / ("game_temp.dll")).string();

    GameCode gameCode = LoadGameCode(gameDllPath.c_str(), gameDllPathTemp.c_str());

    if (!gameCode.valid)
    {
        // TODO: Log error.
    }

    Window window = Platform::CreateWindow(800, 600, "DeFacto Engine");
    Render::Initialize(&window);

    GameMemory gameMemory = gameCode.GameIntialize();

    double lastTime = Platform::GetCurrentTime();
    g_running       = true;

    while (g_running && !Platform::WindowClosed(&window))
    {
        Platform::FileWriteTime lastGameDllWriteTime =
          Platform::GetFileWriteTime(gameDllPath.c_str());

        if (gameCode.library.lastWriteTime != lastGameDllWriteTime)
        {
            UnloadGameCode(&gameCode);
            gameCode = LoadGameCode(gameDllPath.c_str(), gameDllPathTemp.c_str());
            gameCode.GameReload(gameMemory);
        }

        double currentTime = Platform::GetCurrentTime();
        float deltaTime    = (float)(currentTime - lastTime);
        lastTime           = currentTime;

        Render::BeginFrame();

        // TODO: GameUpdate call should be moved above, before begin frame and physics update
        // (later). Now it is here, because it calls DrawQuad which creates and bind vao after
        // glClear.
        gameCode.GameUpdate(gameMemory, deltaTime);

        Render::EndFrame();

        g_running = !Platform::KeyPressed(Key::ESC);
    }

    UnloadGameCode(&gameCode);

    return 0;
}
