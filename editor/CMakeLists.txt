cmake_minimum_required(VERSION 3.0.0)
project(editor)

set(CMAKE_CXX_STANDARD 17)

find_package(Bullet REQUIRED)
find_package(Lua REQUIRED)

add_executable(${PROJECT_NAME}
    src/editor.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${BULLET_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
        ../extern/entt/single_include
        ../extern/imgui
        ../extern/json/single_include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        engine
        ${BULLET_LIBRARIES}
        ${LUA_LIBRARIES}
)
