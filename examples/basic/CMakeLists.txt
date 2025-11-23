project(basic_example)

set(EXE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

file(GLOB GAME_SOURCES "main.cpp")
add_executable(${PROJECT_NAME} ${GAME_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXE_OUTPUT_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE "defacto-engine")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/engine/include)

# Copy static resources
file(COPY "${CMAKE_CURRENT_LIST_DIR}/resources/images/" DESTINATION "${EXE_OUTPUT_DIR}/resources/images/")
file(COPY "${CMAKE_CURRENT_LIST_DIR}/resources/models/" DESTINATION "${EXE_OUTPUT_DIR}/resources/models/")
