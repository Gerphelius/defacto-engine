target_sources(${PROJECT_NAME}
    PRIVATE
        input_glfw.cpp
        window_glfw.cpp

    PRIVATE
        input_glfw.hpp
        window_glfw.hpp
)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# TODO: move to shared libraries, or make a standalone logger and use spdlog there
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
