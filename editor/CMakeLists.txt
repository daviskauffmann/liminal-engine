cmake_minimum_required(VERSION 3.0.0)
project(editor)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
    src/editor.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${GLM_INCLUDE_DIRS}
        ../extern/entt/single_include
        ../extern/imgui
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        engine
)
