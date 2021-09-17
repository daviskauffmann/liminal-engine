cmake_minimum_required(VERSION 3.0.0)
project(liminal)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/extern/sdl2-cmake-modules")

find_package(assimp REQUIRED)
find_package(Bullet REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(Lua REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_executable(${PROJECT_NAME}
    src/audio/sound.cpp
    src/components/audio_source.cpp
    src/components/directional_light.cpp
    src/components/point_light.cpp
    src/components/script.cpp
    src/components/spot_light.cpp
    src/components/sprite.cpp
    src/components/terrain.cpp
    src/components/transform.cpp
    src/components/water.cpp
    src/core/assets.cpp
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
    src/main.cpp
    src/stb_image.cpp
    src/stb_include.cpp)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${ASSIMP_INCLUDE_DIRS}
        ${BULLET_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
        extern/entt/single_include
        extern/imgui
        extern/imgui/backends
        extern/json/single_include
        extern/stb
)

target_link_libraries(${PROJECT_NAME} 
    ${ASSIMP_LIBRARIES}
    ${BULLET_LIBRARIES}
    GLEW::GLEW
    ${LUA_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${OPENAL_LIBRARY}
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)
