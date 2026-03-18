#pragma once

namespace DF::PLATFORM
{
struct Window
{
    void* handle;
};

struct Size
{
    int width;
    int height;
};

Window CreateWindow(int width, int height, const char* title);
void SwapBuffers(Window* window);
bool WindowClosed(Window* window);
Size GetFramebufferSize(Window* window);

} // namespace DF::PLATFORM