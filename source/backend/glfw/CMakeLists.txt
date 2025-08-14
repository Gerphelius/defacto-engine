target_sources(${PROJECT_NAME}
    PRIVATE
        input.cpp
        window.cpp

    PRIVATE
        input.hpp
        window.hpp
)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
