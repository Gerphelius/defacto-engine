project("defacto-engine")

# Create engine library
add_library(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(assets)
add_subdirectory(backend)
add_subdirectory(backend/glfw)
add_subdirectory(components)
add_subdirectory(core)
add_subdirectory(input)
add_subdirectory(math)
add_subdirectory(observer)
add_subdirectory(render)
add_subdirectory(ui_debug)
add_subdirectory(ui_debug/world_outliner)
add_subdirectory(utils)
add_subdirectory(world)

# Copy static resources
file(COPY "${CMAKE_SOURCE_DIR}/resources/images/" DESTINATION "${CMAKE_BINARY_DIR}/resources/images/")
file(COPY "${CMAKE_SOURCE_DIR}/resources/models/" DESTINATION "${CMAKE_BINARY_DIR}/resources/models/")

# Handle shader files
file(GLOB SHADER_FILES "${CMAKE_SOURCE_DIR}/resources/shaders/*.glsl")

set(SHADER_DESTINATION_DIR "${CMAKE_BINARY_DIR}/shaders")

foreach(SHADER_FILE ${SHADER_FILES})
    # Get the relative path of the shader file with respect to the resources directory
    file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}" ${SHADER_FILE})
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

# find_package(assimp CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

# Set consistent runtime library for hot reload
# set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


if (MSVC)
    add_compile_options(/W4 /WX)
    set_property(TARGET ${PROJECT_NAME} PROPERTY COMPILE_WARNING_AS_ERROR ON)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()
