cmake_minimum_required(VERSION 3.0.0)
project(minecraft)

set(CMAKE_CXX_STANDARD 17)

find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    src/blocks/air_block.cpp
    src/blocks/grass_block.cpp
    src/blocks/stone_block.cpp
    src/block.cpp
    src/chunk.cpp
    src/mesh_data.cpp
    src/minecraft.cpp
    src/world.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glm::glm
        imgui::imgui
        liminal_engine
)
