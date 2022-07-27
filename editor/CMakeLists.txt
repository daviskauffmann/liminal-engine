cmake_minimum_required(VERSION 3.0.0)
project(liminal_editor)

set(CMAKE_CXX_STANDARD 20)
set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc")

find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)
find_package(unofficial-nativefiledialog CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    src/editor.cpp
    ${APP_ICON_RESOURCE_WINDOWS}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glm::glm
        imgui::imgui
        imguizmo::imguizmo
        liminal_engine
        unofficial::nativefiledialog::nfd
)
