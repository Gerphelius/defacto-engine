target_sources(${PROJECT_NAME}
    PRIVATE
        world.cpp

    PUBLIC
        world.hpp
)

# TODO: Remove EnTT
find_package(EnTT CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE EnTT::EnTT)

find_package(flecs CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE flecs::flecs_static)
