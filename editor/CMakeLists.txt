cmake_minimum_required(VERSION 3.0.0)

project(liminal_editor LANGUAGES CXX)

find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)
find_package(unofficial-nativefiledialog CONFIG REQUIRED)

file(
    GLOB_RECURSE SOURCE_FILES
    CONFIGURE_DEPENDS
    SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp
)
set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc")
add_executable(${PROJECT_NAME} 
    ${SOURCE_FILES} 
    ${APP_ICON_RESOURCE_WINDOWS}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glm::glm
    imgui::imgui
    imguizmo::imguizmo
    liminal_engine
    unofficial::nativefiledialog::nfd
)
