target_sources(${PROJECT_NAME}
    PRIVATE
        renderer.cpp
        shader_program.cpp

    PRIVATE
        renderer.hpp
        shader_program.hpp
        window.hpp
)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

find_package(glad CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)

find_package(SailC++ CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SAIL::sail-c++)
