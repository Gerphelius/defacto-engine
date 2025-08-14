target_sources(${PROJECT_NAME}
    PRIVATE
        ui_manager.cpp
        world_outliner.cpp

    PRIVATE
        ui_manager.hpp
        world_outliner.hpp
        widget.hpp
)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)
