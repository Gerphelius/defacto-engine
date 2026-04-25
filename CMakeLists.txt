cmake_minimum_required(VERSION 3.16)
project("defacto_engine")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXE_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${EXE_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${EXE_OUTPUT_DIR})

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(TRIPLET "x64-windows-static")
set(VCPKG_INSTALLED_DIR "$ENV{VCPKG_INSTALLED}/installed/${TRIPLET}")
set(VCPKG_INCLUDE "${VCPKG_INSTALLED_DIR}/include")
set(VCPKG_LIB "${VCPKG_INSTALLED_DIR}/lib")

set(COMMON_COMPILE_OPTIONS
    /EHsc       # structured exception handling
    /Od         # disable optimization
    /Oi         # enable intrinsics
    /WX         # warnings as errors
    /W4         # warning level 4
    /Zc:nrvo-

    /wd4100     # unreferenced parameter
    /wd4189     # local variable is initialized but not referenced
    /wd4505     # unreferenced local function has been removed
)
set(COMMON_LINK_OPTIONS
    /INCREMENTAL:NO     # disables incremental linking for reducing final size
    /OPT:REF            # eliminates functions and data that are never referenced
)

file(COPY "${CMAKE_CURRENT_LIST_DIR}/resources/" DESTINATION "${EXE_OUTPUT_DIR}/resources/")

# ===================================================================================
#                              ENGINE DLL COMPILATION
# ===================================================================================

set(ENGINE_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/source/engine")
set(EXTERNAL_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/source/external")
set(ENGINE_SOURCE "${CMAKE_SOURCE_DIR}/source/engine/defacto.cpp")

add_library(defacto SHARED ${ENGINE_SOURCE})

target_include_directories(defacto PRIVATE
    ${VCPKG_INCLUDE}
    ${EXTERNAL_INCLUDE_PATH}
    ${ENGINE_INCLUDE_PATH}
)

target_compile_definitions(defacto PRIVATE ENGINE_EXPORTS)
target_compile_options(defacto PRIVATE ${COMMON_COMPILE_OPTIONS})
target_link_options(defacto PRIVATE ${COMMON_LINK_OPTIONS})
target_link_directories(defacto PUBLIC ${VCPKG_LIB})
target_link_libraries(defacto PRIVATE
    shell32.lib
    user32.lib
    gdi32.lib
    opengl32.lib
    glfw3.lib
    glew32.lib
)

# ===================================================================================
#                              GAME DLL COMPILATION
# ===================================================================================

set(GAME_SOURCE "${CMAKE_SOURCE_DIR}/source/example/game.cpp")

configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/compile_dll.cmake"
    "${CMAKE_BINARY_DIR}/compile_dll.cmake"
    @ONLY
)

add_custom_target(game_dll ALL
    COMMAND ${CMAKE_COMMAND}
        -DFILENAME=game
        -DSOURCE_FILE=${GAME_SOURCE}
        -DINCLUDE_ENGINE=${ENGINE_INCLUDE_PATH}
        -DOUT_DIR=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        -DCOMPILER=${CMAKE_CXX_COMPILER}
        -DLINKER=${CMAKE_LINKER}
        -DLIBRARIES=defacto.lib
        -P "${CMAKE_BINARY_DIR}/compile_dll.cmake"
    COMMENT "****** Building game.dll ******"
    VERBATIM
)

## This block is used to add intellisence support for game.dll with unity build
add_library(game_intellisense SHARED ${GAME_SOURCE})
set_target_properties(game_intellisense PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    EXCLUDE_FROM_DEFAULT_BUILD TRUE
)
target_include_directories(game_intellisense PRIVATE
    ${ENGINE_INCLUDE_PATH}
)

# ===================================================================================
#                              MAIN COMPILATION
# ===================================================================================

add_executable(main "source/main.cpp")

target_include_directories(main PRIVATE ${ENGINE_INCLUDE_PATH})
target_compile_options(main PRIVATE ${COMMON_COMPILE_OPTIONS})
target_link_options(main PRIVATE ${COMMON_LINK_OPTIONS})

target_link_libraries(main PRIVATE
    defacto
)
