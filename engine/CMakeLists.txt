cmake_minimum_required(VERSION 3.0.0)
project(liminal_engine)

set(CMAKE_CXX_STANDARD 20)

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

add_library(${PROJECT_NAME}
    src/stb_image.cpp
    src/stb_include.cpp
    src/audio/listener.cpp
    src/audio/sound.cpp
    src/audio/source.cpp
    src/components/camera.cpp
    src/components/mesh_renderer.cpp
    src/components/sprite.cpp
    src/components/terrain.cpp
    src/components/transform.cpp
    src/core/al_context.cpp
    src/core/al_device.cpp
    src/core/app.cpp
    src/core/assets.cpp
    src/core/gl_context.cpp
    src/core/glew.cpp
    src/core/imgui_context.cpp
    src/core/renderer.cpp
    src/core/sdl.cpp
    src/core/sdl_image.cpp
    src/core/sdl_mixer.cpp
    src/core/window.cpp
    src/entities/scene.cpp
    src/graphics/cubemap.cpp
    src/graphics/framebuffer.cpp
    src/graphics/mesh.cpp
    src/graphics/model.cpp
    src/graphics/program.cpp
    src/graphics/renderbuffer.cpp
    src/graphics/skybox.cpp
    src/graphics/texture.cpp
    src/physics/rigidbody.cpp
    src/physics/world.cpp
    src/scripting/lua_state.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${LUA_INCLUDE_DIR}
        ${STB_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
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
