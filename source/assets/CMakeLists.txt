target_sources(${PROJECT_NAME}
    PRIVATE
        asset_manager.hpp
        mesh.hpp
        model.hpp
        material.hpp
        texture.hpp

    PRIVATE
        asset_manager.cpp
        mesh.cpp
        model.cpp
        material.cpp
        texture.cpp
)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)
