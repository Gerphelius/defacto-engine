target_sources(${PROJECT_NAME}
    PRIVATE
        ui_manager.cpp

    PRIVATE
        ui_manager.hpp
        widget.hpp
)

add_subdirectory(world_outliner)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)
