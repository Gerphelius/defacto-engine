cmake_minimum_required (VERSION 3.30)

# Used to generate compile_commands.json inside build for sourcetrail
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add engine sources subdirectory
add_subdirectory(source)

# Add games as subdirectories
add_subdirectory(examples/basic)
