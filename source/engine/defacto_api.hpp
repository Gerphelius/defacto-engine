#pragma once

#if defined(_WIN32)

#define DF_EXPORT __declspec(dllexport)
#define DF_EXPORT_C extern "C" __declspec(dllexport)

#ifdef ENGINE_EXPORTS
#define DF_API DF_EXPORT
#else
#define DF_API __declspec(dllimport)
#endif

#else

#define DF_API __attribute__((visibility("default")))

#endif

#define Kilobytes(value) ((value) * 1024LL)
#define Megabytes(value) (Kilobytes(value) * 1024LL)
#define Gigabytes(value) (Megabytes(value) * 1024LL)
#define Terabytes(value) (Gigabytes(value) * 1024LL)

#define GAME_INITIALIZE(name) DF::GameMemory name()
#define GAME_RELOAD(name) void name(DF::GameMemory* gameMemory)
#define GAME_UPDATE(name) void name(DF::GameMemory* gameMemory, float dt)

#include "platform/platform_os.hpp"

namespace DF
{

struct Arena
{
    void* base;
    int offset;
    int size;
};

struct GameMemory
{
    Arena permanent;
    Arena transient;
};

DF_API void* ArenaPush(Arena* arena, int size);

struct Window
{
    void* handle;
};

struct Size
{
    int width;
    int height;
};

struct Color
{
    float r;
    float g;
    float b;
    float a;
};

enum class Key
{
    UNDEFINED,
    ESC,
    SPACE,
    ENTER,
    SHIFT_L,
    N,
    W,
    S,
    A,
    D,
    Q,
    E,
    UP,
    DOWN,
    LEFT,
    RIGHT,

    MAX_KEYS,
};

enum class MouseKey
{
    UNDEFINED,
    LEFT,
    RIGHT,
    MIDDLE,

    MAX_KEYS,
};

enum class KeyState
{
    UNKNOWN,
    PRESSED,
    RELEASED,
    HOLD,
};

struct Vec2
{
    float x;
    float y;
};

struct String
{
    char* data;
    int length;
};

DF_API String StrFormat(Arena* buffer, const char* str, ...);

DF_API float Map(float value, float low1, float high1, float low2, float high2);
DF_API int RandomInt(int min, int max);

namespace Platform
{

DF_API Window CreateWindow(int width, int height, const char* title);
DF_API bool WindowClosed(Window* window);
DF_API Size GetFramebufferSize();
DF_API bool MouseKeyPressed(MouseKey key);
DF_API bool KeyPressed(Key key);
DF_API Vec2 GetCursorPos();
DF_API double GetCurrentTime();
DF_API Vec2 GetScrollPos();
DF_API Arena AllocateMemory(size_t size);

} // namespace Platform

typedef uint32_t Texture;

struct Font
{
    Texture bitmap;
    float size;
    float atlasWidth;
    float atlasHeight;
    float lineHeight;
    float distanceRange;

    struct Glyph
    {
        int code;
        float advance;

        struct AtlasBounds
        {
            float top;
            float right;
            float bottom;
            float left;
        };
        AtlasBounds atlasBounds;

        struct PlaneBounds
        {
            float top;
            float right;
            float bottom;
            float left;
        };
        PlaneBounds planeBounds;
    };
    int glyphCount;
    Glyph* glyphs;
};

DF_API Font* LoadFont(const char* pathJson, const char* pathBitmap, Arena* arena);

namespace Render
{

struct UVMap
{
    Vec2 bottomLeft  = { 0.0f, 0.0f };
    Vec2 topLeft     = { 0.0f, 1.0f };
    Vec2 topRight    = { 1.0f, 1.0f };
    Vec2 bottomRight = { 1.0f, 0.0f };
};

DF_API void Initialize(Window* window);
DF_API void BeginFrame();
DF_API void EndFrame();
DF_API void DrawQuad(Vec2 pos, Size size, Color color, UVMap uvMap = {});
DF_API void DrawText(const Font* font, const char* str, int fontSize, Vec2 pos, int strlen = 0);
DF_API void BeginScissor(int x, int y, int width, int height);
DF_API void EndScissor();

} // namespace Render

} // namespace DF
