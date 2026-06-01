## Defacto Engine

A lightweight game engine written in C/C++ focused on simplicity, iteration speed and data-oriented design.

### Overview
**This is an educational project and it is not production ready!**

The project is build with hot-reload in mind for rapid development. Currently only Game module is hot-reloadable, but support for the Engine module may be added in the future.
It also uses unity (jumbo) build to help the compiler in code optimizations, reduce the number of header files and improve build speed.
There are 3 translation units:
* Main: Entry point that initializes other modules and runs the game loop. It dynamically loads game.dll at runtime and is the only module that can allocate and store persistent memory.
* Engine: Core engine features including rendering, input, assets, math, and memory management. Built as a shared library but linked statically to both Main and Game.
* Game: game related code. Built as a shared library to enable hot-reloading.

Third-Party Libraries
--------------------

| Technology        | Component                                                     |
| ----------------- | ------------------------------------------------------------- |
| **C++20**         | Engine Core                                                   |
| [**CMake**]       | Build System                                                  |
| [**OpenGL**]      | Rendering, Shaders                                            |
| [**GLFW**]        | Window and Input                                              |
| [**Clay**]        | UI                                                            |
| [**stb_image**]   | Image loading                                                 |
| [**stb_sprintf**] | High performance sprintf                                      |

[**CMake**]: https://cmake.org/
[**OpenGL**]: https://www.opengl.org/
[**GLFW**]: https://www.glfw.org
[**Clay**]: https://www.nicbarker.com/clay
[**stb_image**]: https://github.com/nothings/stb/blob/master/stb_image.h
[**stb_sprintf**]: https://github.com/nothings/stb/blob/master/stb_sprintf.h

### Build
**Windows:**
* Setup [**vcpkg**] and install these libraries: `opengl, glfw, glew` as static;
* Update the VCPKG_INSTALLED to your environment variable path name in CMakePresets.json: `"toolchainFile": "$env{VCPKG_INSTALLED}/scripts/buildsystems/vcpkg.cmake"`;
* Configure cmake: `cmake --preset windows-msvc-debug`;
* Build the project: `cmake --build --preset debug`;
* Hot-reload (rebuild only game.dll): `cmake --build --preset hot-reload`;

[**vcpkg**]: https://learn.microsoft.com/en-us/vcpkg/get_started/get-started
