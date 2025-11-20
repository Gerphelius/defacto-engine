target_sources(${PROJECT_NAME}
    PRIVATE
        asset_manager.hpp
        mesh.hpp
        shader.hpp
        model.hpp
        texture.hpp

    PRIVATE
        asset_manager.cpp
        mesh.cpp
        shader.cpp
        model.cpp
        texture.cpp
)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

add_subdirectory(default)
