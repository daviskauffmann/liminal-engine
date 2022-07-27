cmake_minimum_required(VERSION 3.0.0)
project(liminal_player)

set(CMAKE_CXX_STANDARD 20)

find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    src/player.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glm::glm
        imgui::imgui
        liminal_engine
)
