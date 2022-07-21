cmake_minimum_required(VERSION 3.0.0)
project(liminal_player)

set(CMAKE_CXX_STANDARD 17)

find_package(glm CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    src/player.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ../extern/imgui
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glm::glm
        liminal_engine
)
