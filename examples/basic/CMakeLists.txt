project(basic_example)

file(GLOB GAME_SOURCES "main.cpp")

add_executable(${PROJECT_NAME} ${GAME_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE "defacto-engine")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)
