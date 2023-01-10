cmake_minimum_required(VERSION 3.0.0)

project(minecraft LANGUAGES CXX)

find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

file(
    GLOB_RECURSE SOURCE_FILES
    CONFIGURE_DEPENDS
    SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp
)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glm::glm
    imgui::imgui
    liminal_engine
)
