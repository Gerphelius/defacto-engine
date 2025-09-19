target_sources(${PROJECT_NAME}
    PRIVATE
        world.cpp

    PUBLIC
        world.hpp
        object.hpp
)

find_package(flecs CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE flecs::flecs_static)
