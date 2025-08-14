target_sources(${PROJECT_NAME}
    PUBLIC
        math.hpp
        random.hpp
)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm-header-only)
