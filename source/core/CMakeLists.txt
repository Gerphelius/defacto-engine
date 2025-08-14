target_sources(${PROJECT_NAME}
    PRIVATE
        engine.cpp
        world.cpp
        backend_factory.cpp

    PUBLIC
        engine.hpp
        world.hpp

    PRIVATE
        backend_factory.hpp
        service_locator.hpp
)

find_package(EnTT CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE EnTT::EnTT)
