cmake_minimum_required (VERSION 3.25)

# Used to generate compile_commands.json inside build for sourcetrail
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add engine subdirectory
add_subdirectory(engine)

# Add games as subdirectories
add_subdirectory(games/puzzle15)
