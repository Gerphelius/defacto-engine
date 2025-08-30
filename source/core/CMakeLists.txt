target_sources(${PROJECT_NAME}
    PRIVATE
        engine.cpp

    PUBLIC
        engine.hpp

    PRIVATE
        service_locator.hpp
)

find_package(EnTT CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE EnTT::EnTT)
