cmake_minimum_required(VERSION 3.0.0)
project(editor)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    src/editor.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ../extern/entt/single_include
        ../extern/glm
        ../extern/imgui
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        engine
        SDL2::SDL2 SDL2::SDL2main
)
