project("defacto-engine")

file(
    GLOB ENGINE_INCLUDE

    "include/df/input/*.hpp"
    "include/df/core/*.hpp"
    "include/df/render/*.hpp"
    "include/df/entities/*.hpp"
    "include/df/utils/*.hpp"
    "include/df/assets/*.hpp"
)

file(
    GLOB ENGINE_SOURCES

    "src/input/*.cpp"
    "src/core/*.cpp"
    "src/render/*.cpp"
    "src/entities/*.cpp"
    "src/utils/*.cpp"
)

file(GLOB SHADER_FILES "resources/shaders/*.glsl")

# Create engine library
add_library(${PROJECT_NAME} STATIC ${ENGINE_SOURCES} ${ENGINE_INCLUDE})

# Set directories
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/df)

# Copy images
file(COPY "${CMAKE_SOURCE_DIR}/engine/resources/images/" DESTINATION "${CMAKE_BINARY_DIR}/resources/images/")

# Handle shader files
set(SHADER_DESTINATION_DIR "${CMAKE_BINARY_DIR}/shaders")

foreach(SHADER_FILE ${SHADER_FILES})
    # Get the relative path of the shader file with respect to the resources directory
    file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}/engine" ${SHADER_FILE})
    # Determine the destination path
    set(DEST_FILE "${CMAKE_BINARY_DIR}/${REL_PATH}")
    # Create the destination directory
    get_filename_component(DEST_DIR "${DEST_FILE}" DIRECTORY)
    file(MAKE_DIRECTORY "${DEST_DIR}")
    # Copy the shader files to created directory
    add_custom_command(
        OUTPUT ${DEST_FILE}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SHADER_FILE}" "${DEST_FILE}"
        DEPENDS "${SHADER_FILE}"
        COMMENT "~~~~~~~~~~~ Copying shader file ${REL_PATH} ~~~~~~~~~~~"
        VERBATIM
    )
    # Collect all destination files
    list(APPEND SHADER_DEST_FILES "${DEST_FILE}")
endforeach()

add_custom_target(CopyShaders ALL DEPENDS ${SHADER_DEST_FILES})
add_dependencies(${PROJECT_NAME} CopyShaders)

# Find and link dependencies
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm-header-only)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

find_package(glad CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)

find_package(SailC++ CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SAIL::sail-c++)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

# find_package(Boost REQUIRED COMPONENTS signals2)
# target_link_libraries(${PROJECT_NAME} PRIVATE Boost::signals2)

# Set consistent runtime library for hot reload
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()
