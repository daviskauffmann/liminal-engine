cmake_minimum_required(VERSION 3.0.0)
project(engine)

set(CMAKE_CXX_STANDARD 17)

find_package(assimp REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
find_package(Bullet CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(Lua REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2-image CONFIG REQUIRED)
find_package(SDL2-mixer CONFIG REQUIRED)

add_library(${PROJECT_NAME}
    src/audio/sound.cpp
    src/audio/source.cpp
    src/components/audio_listener.cpp
    src/components/physical.cpp
    src/components/script.cpp
    src/components/sprite.cpp
    src/components/terrain.cpp
    src/components/transform.cpp
    src/components/water.cpp
    src/core/assets.cpp
    src/core/engine.cpp
    src/core/platform.cpp
    src/core/scene.cpp
    src/graphics/camera.cpp
    src/graphics/cubemap.cpp
    src/graphics/mesh.cpp
    src/graphics/model.cpp
    src/graphics/program.cpp
    src/graphics/renderer.cpp
    src/graphics/skybox.cpp
    src/graphics/texture.cpp
    src/graphics/vertex.cpp
    src/imgui.cpp
    src/stb_image.cpp
    src/stb_include.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${LUA_INCLUDE_DIR}
        ../extern/entt/single_include
        ../extern/glm
        ../extern/imgui
        ../extern/imgui/backends
        ../extern/json/single_include
        ../extern/sol2/single/include
        ../extern/stb
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        assimp::assimp
        BulletSoftBody BulletDynamics BulletCollision Bullet3Common LinearMath
        GLEW::GLEW
        ${LUA_LIBRARIES}
        OpenAL::OpenAL
        SDL2::SDL2 SDL2::SDL2main
        SDL2::SDL2_image
        SDL2::SDL2_mixer
)
