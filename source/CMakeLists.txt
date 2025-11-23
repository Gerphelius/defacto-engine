project("defacto-engine")

# Create engine library
add_library(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(assets)
add_subdirectory(backend)
add_subdirectory(components)
add_subdirectory(core)
add_subdirectory(input)
add_subdirectory(math)
add_subdirectory(observer)
add_subdirectory(render)
add_subdirectory(ui_debug)
add_subdirectory(utils)
add_subdirectory(world)

# Handle shader files
set(GENERATED_SHADER_HPP "${CMAKE_BINARY_DIR}/generated/shaders.hpp")
file(GLOB SHADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/*.glsl")

set(IDX 0)
foreach(FILE ${SHADER_FILES})
    list(APPEND SHADER_FILES_LIST "-D SHADER_FILE_${IDX}=${FILE}")
    math(EXPR IDX "${IDX} + 1")
endforeach()

message("********* List: ${SHADER_FILES_LIST}")
message("********* CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

add_custom_command(
    OUTPUT "${GENERATED_SHADER_HPP}"
    COMMAND ${CMAKE_COMMAND}
        ${SHADER_FILES_LIST}
        -D OUTFILE="${GENERATED_SHADER_HPP}"
        -D SHADERS_COUNT=${IDX}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/generated_shaders.cmake
        > "${CMAKE_BINARY_DIR}/shader_gen_stderr.txt"
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/generated_shaders.cmake
    DEPENDS ${SHADER_FILES}
    COMMENT "**** Generating shaders.hpp ****"
)

add_custom_target(GenerateShaders DEPENDS "${GENERATED_SHADER_HPP}")
add_dependencies(${PROJECT_NAME} GenerateShaders)
target_sources(${PROJECT_NAME} PRIVATE "${GENERATED_SHADER_HPP}")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}/generated")

if (MSVC)
    add_compile_options(/W4 /WX)
    set_property(TARGET ${PROJECT_NAME} PROPERTY COMPILE_WARNING_AS_ERROR ON)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()
