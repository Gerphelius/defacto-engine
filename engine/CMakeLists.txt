project("defacto-engine")

file(
    GLOB ENGINE_INCLUDE

    "include/df/input/*.h"
    "include/df/core/*.h"
    "include/df/render/*.h"
    "include/df/utils/*.h"
)

file(
    GLOB ENGINE_SOURCES

    "src/input/*.cpp"
    "src/core/*.cpp"
    "src/render/*.cpp"
    "src/utils/*.cpp"
)

# Create engine library
add_library(${PROJECT_NAME} STATIC ${ENGINE_SOURCES} ${ENGINE_INCLUDE})

# Set include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/df)

# Copy assets to build directory
# TODO: remove this after creating better resource management
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/ DESTINATION ${CMAKE_BINARY_DIR}/assets)

# Find and link dependencies
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm-header-only)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

find_package(glad CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)

find_package(Boost REQUIRED COMPONENTS signals2)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::signals2)

# Set consistent runtime library
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()
