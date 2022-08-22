cmake_minimum_required(VERSION 3.0.0)

project(liminal_engine LANGUAGES CXX)

file(
    GLOB_RECURSE SOURCE_FILES
    SOURCES ${PROJECT_SOURCE_DIR}/**/*.cpp
)
add_library(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${LUA_INCLUDE_DIR}
        ${STB_INCLUDE_DIRS}
)

find_package(assimp REQUIRED)
find_package(Bullet CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Lua REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2-image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")
target_link_libraries(${PROJECT_NAME} PRIVATE
    assimp::assimp
    BulletSoftBody BulletDynamics BulletCollision Bullet3Common LinearMath
    EnTT::EnTT
    GLEW::GLEW
    glm::glm
    imgui::imgui
    ${LUA_LIBRARIES}
    nlohmann_json nlohmann_json::nlohmann_json
    OpenAL::OpenAL
    SDL2::SDL2 SDL2::SDL2main
    SDL2::SDL2_image
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    sol2
    spdlog::spdlog
)
