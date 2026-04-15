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

#define EXPORT __attribute__((visibility("default")))

#endif

#define GAME_INITIALIZE(name) DF::GameMemory name()
#define GAME_RELOAD(name) void name(DF::GameMemory gameMemory)
#define GAME_UPDATE(name) void name(DF::GameMemory gameMemory, float dt)

#include "platform/platform_os.hpp"

namespace DF
{

struct GameMemory
{
    uint64_t permanentStorageSize;
    void* permanentStorage;

    uint64_t transientStorageSize;
    void* transientStorage;
};

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

namespace Math
{
struct Vec2
{
    float x;
    float y;
};

float Map(float value, float low1, float high1, float low2, float high2);
} // namespace Math

namespace Platform
{

DF_API Window CreateWindow(int width, int height, const char* title);
DF_API bool WindowClosed(Window* window);
DF_API Size GetFramebufferSize();
DF_API bool MouseKeyPressed(MouseKey key);
DF_API bool KeyPressed(Key key);
DF_API Math::Vec2 GetCursorPos();
DF_API double GetCurrentTime();
DF_API Math::Vec2 GetScrollPos();

} // namespace Platform

namespace Assets
{

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

DF_API bool LoadFont(const char* pathJson, const char* pathBitmap, Font* memory);

} // namespace Assets

namespace Render
{

struct UVMap
{
    Math::Vec2 bottomLeft  = { 0.0f, 0.0f };
    Math::Vec2 topLeft     = { 0.0f, 1.0f };
    Math::Vec2 topRight    = { 1.0f, 1.0f };
    Math::Vec2 bottomRight = { 1.0f, 0.0f };
};

DF_API void Initialize(Window* window);
DF_API void BeginFrame();
DF_API void EndFrame();
DF_API void DrawQuad(Math::Vec2 pos, Size size, Color color, UVMap uvMap = {});
DF_API void DrawText(const Assets::Font* font,
                     const char* str,
                     int fontSize,
                     Math::Vec2 pos,
                     int strlen = 0);
DF_API void BeginScissor(int x, int y, int width, int height);
DF_API void EndScissor();

} // namespace Render

namespace UI
{

// DF_API void Initialize();
// DF_API void Render(int width, int height, Math::Vec2 mousePos, bool lmbPressed, float deltaTime);
//
// struct BlockConfig
//{
// };
//
// struct TextConfig
//{
// };
//
// struct ImageConfig
//{
// };
//
// struct Node
//{
//     enum class Type
//     {
//         BLOCK,
//         TEXT,
//         IMAGE,
//     };
//
//     union Config
//     {
//         BlockConfig block;
//         TextConfig text;
//         ImageConfig image;
//     };
//
//     Type type;
//     const char* id;
//     Config config;
//     Node* first_child;
//     Node* next_sibling;
//     Node* parent;
// };
//
// Node* CreateElement(Node::Type type, const char* id);
// void AppendChild(Node* parent, Node* child);
// void RenderUI(const Node* root);

} // namespace UI

} // namespace DF
