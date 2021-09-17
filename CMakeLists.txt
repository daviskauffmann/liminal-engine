cmake_minimum_required(VERSION 3.0.0)
project(liminal)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/extern/sdl2-cmake-modules")

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

include_directories(${PROJECT_NAME}
    extern/entt/single_include
    extern/imgui
    extern/imgui/backends
    extern/json/single_include
    extern/stb
)

find_package(assimp REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${ASSIMP_LIBRARIES})

find_package(Bullet REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${BULLET_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${BULLET_LIBRARIES})

find_package(GLEW REQUIRED)
target_link_libraries(${PROJECT_NAME} GLEW::GLEW)

find_package(GLM REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLM_INCLUDE_DIRS})

find_package(Lua REQUIRED)
target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES})

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})

find_package(OpenAL REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OPENAL_LIBRARY})

find_package(SDL2 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

find_package(SDL2_image REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_IMAGE_LIBRARIES})

find_package(SDL2_mixer REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_MIXER_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_MIXER_LIBRARIES})
