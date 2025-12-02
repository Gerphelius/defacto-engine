
if (MSVC)
  # Force Edit and Continue
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT EditAndContinue)
  
  # Add /INCREMENTAL for linker in Debug mode
  add_compile_options("$<$<CONFIG:Debug>:/ZI>")
  add_link_options("$<$<CONFIG:Debug>:/INCREMENTAL>")
endif()

project(basic_example)

set(EXE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

file(GLOB GAME_SOURCES "main.cpp")
add_executable(${PROJECT_NAME} ${GAME_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXE_OUTPUT_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE "defacto-engine")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

target_sources(${PROJECT_NAME} PRIVATE test_widget.hpp)

# Copy static resources
file(COPY "${CMAKE_CURRENT_LIST_DIR}/resources/images/" DESTINATION "${EXE_OUTPUT_DIR}/resources/images/")
file(COPY "${CMAKE_CURRENT_LIST_DIR}/resources/models/" DESTINATION "${EXE_OUTPUT_DIR}/resources/models/")
