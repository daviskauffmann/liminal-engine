cmake_minimum_required(VERSION 3.0.0)
project(liminal_editor)

set(CMAKE_CXX_STANDARD 17)
set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc")

find_package(EnTT CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    src/editor.cpp
    ${APP_ICON_RESOURCE_WINDOWS}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ../extern/imgui
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        EnTT::EnTT
        glm::glm
        liminal_engine
        SDL2::SDL2 SDL2::SDL2main
)
