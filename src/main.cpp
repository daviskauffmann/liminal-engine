#include <bullet/btBulletDynamicsCommon.h>
#include <cxxopts.hpp>
#include <entt/entt.hpp>
#include <fstream>
#include <iostream>
#include <imgui.h>
#include <nlohmann/json.hpp>
#include <SDL2/SDL.h>
#include <sol/sol.hpp>

#include "components/audio_source.hpp"
#include "components/directional_light.hpp"
#include "components/mesh_renderer.hpp"
#include "components/point_light.hpp"
#include "components/script.hpp"
#include "components/spot_light.hpp"
#include "components/terrain.hpp"
#include "components/transform.hpp"
#include "components/water.hpp"
#include "audio.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "platform.hpp"
#include "renderer.hpp"
#include "skybox.hpp"
#include "sound.hpp"

#define VERSION "v0.0.1"

#define WINDOW_TITLE "Liminal Engine"

// TODO: this function is kinda weirding me out
void lua_set_functions(sol::state &lua, entt::registry &registry, liminal::renderer &renderer)
{
    lua["SetSkybox"] = [&renderer](const std::string &filename) -> void
    {
        renderer.skybox = new liminal::skybox(filename);
    };
    lua["AddEntity"] = [&registry]() -> entt::entity
    {
        return registry.create();
    };
    lua["AddTransform"] = [&registry](entt::entity entity, float x, float y, float z, float rx, float ry, float rz, float sx, float sy, float sz) -> void
    {
        registry.emplace<liminal::transform>(entity, nullptr, glm::vec3(x, y, z), glm::vec3(rx, ry, rz), glm::vec3(sx, sy, sz));
    };
    lua["UpdateTransform"] = [&registry](entt::entity entity, float x, float y, float z, float rx, float ry, float rz, float sx, float sy, float sz) -> void
    {
        auto &transform = registry.get<liminal::transform>(entity);
        transform.position = glm::vec3(x, y, z);
        transform.rotation = glm::vec3(rx, ry, rz);
        transform.scale = glm::vec3(sx, sy, sz);
    };
    lua["AddMeshRenderer"] = [&registry](entt::entity entity, const std::string &filename, bool flip_uvs) -> void
    {
        registry.emplace<liminal::mesh_renderer>(entity, new liminal::model(filename, flip_uvs));
    };
    lua["UpdateMeshRenderer"] = [&registry](entt::entity entity, const std::string &filename, bool flip_uvs) -> void
    {
        auto &mesh_renderer = registry.get<liminal::mesh_renderer>(entity);
        mesh_renderer.model = new liminal::model(filename, flip_uvs);
    };
    lua["AddPointLight"] = [&registry](entt::entity entity, float r, float g, float b) -> void
    {
        registry.emplace<liminal::point_light>(entity, glm::vec3(r, g, b), 512);
    };
    lua["UpdatePointLight"] = [&registry](entt::entity entity, float r, float g, float b) -> void
    {
        auto &point_light = registry.get<liminal::point_light>(entity);
        point_light.color = glm::vec3(r, g, b);
    };
    lua["AddScript"] = [&registry, &renderer](entt::entity entity, const std::string &filename) -> void
    {
        sol::state *lua = new sol::state();
        lua->open_libraries(sol::lib::base, sol::lib::math);
        lua_set_functions(*lua, registry, renderer);
        lua->script_file(filename);
        registry.emplace<liminal::script>(entity, lua);
    };
}

int main(int argc, char *argv[])
{
    // parse command line options
    int window_width;
    int window_height;
    int render_scale;

    try
    {
        cxxopts::Options options("liminal");

        cxxopts::OptionAdder option_adder = options.add_options();
        option_adder("height", "Set window height", cxxopts::value<int>()->default_value("720"));
        option_adder("h,help", "Print usage");
        option_adder("scale", "Set render scale", cxxopts::value<float>()->default_value("1.0"));
        option_adder("v,version", "Print version");
        option_adder("width", "Set window width", cxxopts::value<int>()->default_value("1280"));

        cxxopts::ParseResult result = options.parse(argc, argv);

        window_height = result["height"].as<int>();

        if (result.count("help"))
        {
            std::cout << options.help() << std::endl;
            return 0;
        }

        render_scale = glm::clamp(result["scale"].as<float>(), 0.1f, 1.0f);

        if (result.count("version"))
        {
            std::cout << VERSION << std::endl;
            return 0;
        }

        window_width = result["width"].as<int>();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // init subsystems
    liminal::platform platform(WINDOW_TITLE, window_width, window_height);
    liminal::renderer renderer(
        window_width, window_height, render_scale,
        window_width, window_height,
        window_width, window_height);
    liminal::audio audio;
    entt::registry registry;

    // get imgui reference
    ImGuiIO &io = ImGui::GetIO();

    // init physics
    btDefaultCollisionConfiguration *collision_configuration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher *dispatcher = new btCollisionDispatcher(collision_configuration);
    btBroadphaseInterface *overlapping_pair_cache = new btDbvtBroadphase();
    btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver();
    btDiscreteDynamicsWorld *world = new btDiscreteDynamicsWorld(dispatcher, overlapping_pair_cache, solver, collision_configuration);

    world->setGravity(btVector3(0.0f, -9.8f, 0.0f));

    // load assets
    // TODO: asset management
    liminal::sound ambient_sound("assets/audio/ambient.wav");
    liminal::sound bounce_sound("assets/audio/bounce.wav");
    liminal::sound shoot_sound("assets/audio/shoot.wav");

    // load scene
    std::ifstream stream("assets/scenes/main.json");
    nlohmann::json json;
    stream >> json;
    for (auto &[key, value] : json.items())
    {
        if (key == "skybox")
        {
            renderer.skybox = new liminal::skybox(json["skybox"]);
        }

        if (key == "entities")
        {
            for (auto &element : value)
            {
                auto entity = registry.create();

                for (auto &[key, value] : element.items())
                {
                    if (key == "transform")
                    {
                        glm::vec3 position(
                            value["position"]["x"],
                            value["position"]["y"],
                            value["position"]["z"]);
                        glm::vec3 rotation(
                            value["rotation"]["x"],
                            value["rotation"]["y"],
                            value["rotation"]["z"]);
                        glm::vec3 scale(
                            value["scale"]["x"],
                            value["scale"]["y"],
                            value["scale"]["z"]);
                        registry.emplace<liminal::transform>(entity, nullptr, position, rotation, scale);
                    }

                    if (key == "mesh_renderer")
                    {
                        std::string filename(value["filename"]);
                        bool flip_uvs = value["flip_uvs"];
                        registry.emplace<liminal::mesh_renderer>(entity, new liminal::model(filename, flip_uvs));
                    }

                    if (key == "script")
                    {
                        sol::state *lua = new sol::state();
                        lua->open_libraries(sol::lib::base, sol::lib::math);
                        lua_set_functions(*lua, registry, renderer);
                        lua->script_file(value["filename"]);
                        registry.emplace<liminal::script>(entity, lua);
                    }
                }
            }
        }
    }

    const float sun_intensity = 10.0f;
    auto sun = registry.create();
    registry.emplace<liminal::directional_light>(sun, glm::vec3(0.352286f, -0.547564f, -0.758992f), glm::vec3(1.0f, 1.0f, 1.0f) * sun_intensity, 4096);

    const float flashlight_intensity = 20.0f;
    auto flashlight = registry.create();
    registry.emplace<liminal::transform>(flashlight, nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    registry.emplace<liminal::spot_light>(flashlight, glm::vec3(1.0f, 1.0f, 1.0f) * flashlight_intensity, cosf(glm::radians(12.5f)), cosf(glm::radians(15.0f)), 1024);

    auto water = registry.create();
    registry.emplace<liminal::water>(water, glm::vec3(0.0f, -2.0f, 0.0f), 100.0f);

    // auto terrain = registry.create();
    // registry.emplace<liminal::terrain>(terrain, "assets/images/heightmap.png", glm::vec3(0.0f, 0.0f, 0.0f), 100.0f, 10.0f);

    auto ambience = registry.create();
    registry.emplace<liminal::transform>(ambience);
    registry.emplace<liminal::audio_source>(ambience);
    registry.get<liminal::audio_source>(ambience).set_loop(true);
    registry.get<liminal::audio_source>(ambience).set_gain(0.25f);
    // registry.get<liminal::audio_source>(ambience).play(ambient_sound);

    auto bounce = registry.create();
    registry.emplace<liminal::transform>(bounce);
    registry.emplace<liminal::audio_source>(bounce);

    auto weapon = registry.create();
    registry.emplace<liminal::transform>(weapon);
    registry.emplace<liminal::audio_source>(weapon);

    auto camera = renderer.camera = new liminal::camera(
        glm::vec3(0.0f, 0.0f, 3.0f),
        0.0f,
        -90.0f,
        0.0f,
        45.0f);

    unsigned int current_time = 0;
    float time_scale = 1.0f;
    bool console_open = false;
    bool edit_mode = false;
    bool flashlight_on = true;
    bool flashlight_follow = true;

    SDL_SetRelativeMouseMode(SDL_TRUE);

    bool quit = false;
    while (!quit)
    {
        // calculate time between frames
        unsigned int previous_time = current_time;
        current_time = SDL_GetTicks();
        float delta_time = ((current_time - previous_time) / 1000.0f) * time_scale;

        // gather input
        int num_keys;
        const unsigned char *keys = SDL_GetKeyboardState(&num_keys);
        int mouse_x, mouse_y;
        unsigned int mouse = SDL_GetMouseState(&mouse_x, &mouse_y);

        // process events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            platform.process_event(&event);

            switch (event.type)
            {
            case SDL_QUIT:
            {
                quit = true;
            }
            break;
            case SDL_WINDOWEVENT:
            {
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                {
                    window_width = event.window.data1;
                    window_height = event.window.data2;
                    platform.set_window_size(window_width, window_height);
                    renderer.set_screen_size(window_width, window_height, render_scale);
                    renderer.set_reflection_size(window_width, window_height);
                    renderer.set_refraction_size(window_width, window_height);
                    std::cout << "Window resized to " << window_width << "x" << window_height << std::endl;
                }
                break;
                }
            }
            break;
            case SDL_KEYDOWN:
            {
                if (!io.WantCaptureKeyboard)
                {
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_TAB:
                    {
                        SDL_SetRelativeMouseMode((SDL_bool)!SDL_GetRelativeMouseMode());
                    }
                    break;
                    case SDLK_RETURN:
                    {
                        if (keys[SDL_SCANCODE_LALT])
                        {
                            platform.toggle_fullscreen();
                        }
                    }
                    break;
                    case SDLK_MINUS:
                    {
                        if (render_scale > 0.2f)
                        {
                            render_scale -= 0.1f;
                        }
                        renderer.set_screen_size(window_width, window_height, render_scale);
                        std::cout << "Render scale changed to " << render_scale << std::endl;
                    }
                    break;
                    case SDLK_EQUALS:
                    {
                        if (render_scale < 1.0f)
                        {
                            render_scale += 0.1f;
                        }
                        renderer.set_screen_size(window_width, window_height, render_scale);
                        std::cout << "Render scale changed to " << render_scale << std::endl;
                    }
                    break;
                    case SDLK_BACKQUOTE:
                    {
                        console_open = !console_open;
                    }
                    break;
                    case SDLK_e:
                    {
                        edit_mode = !edit_mode;
                    }
                    break;
                    case SDLK_f:
                    {
                        flashlight_on = !flashlight_on;
                    }
                    break;
                    case SDLK_g:
                    {
                        flashlight_follow = !flashlight_follow;
                    }
                    break;
                    case SDLK_r:
                    {
                        renderer.reload_programs();
                    }
                    break;
                    case SDLK_t:
                    {
                        if (time_scale > 0.25f)
                        {
                            time_scale = 0.25f;
                        }
                        else
                        {
                            time_scale = 1.0f;
                        }
                    }
                    break;
                    case SDLK_F4:
                    {
                        if (keys[SDL_SCANCODE_LALT])
                        {
                            quit = true;
                        }
                    }
                    break;
                    }
                }
            }
            break;
            case SDL_MOUSEMOTION:
            {
                if (!io.WantCaptureMouse)
                {
                    if (SDL_GetRelativeMouseMode())
                    {
                        camera->pitch -= event.motion.yrel * 0.1f;
                        camera->yaw += event.motion.xrel * 0.1f;
                        if (camera->pitch > 89.0f)
                        {
                            camera->pitch = 89.0f;
                        }
                        if (camera->pitch < -89.0f)
                        {
                            camera->pitch = -89.0f;
                        }
                    }
                }
            }
            break;
            case SDL_MOUSEWHEEL:
            {
                if (!io.WantCaptureMouse)
                {
                    if (SDL_GetRelativeMouseMode())
                    {
                        camera->fov -= event.wheel.y;
                        if (camera->fov <= 1.0f)
                        {
                            camera->fov = 1.0f;
                        }
                        if (camera->fov >= 120.0f)
                        {
                            camera->fov = 120.0f;
                        }
                    }
                }
            }
            break;
            }
        }

        glm::vec3 camera_front = camera->calc_front();
        glm::vec3 camera_right = camera->calc_right();

        static glm::vec3 velocity(0.0f, 0.0f, 0.0f);
        glm::vec3 acceleration(0.0f, 0.0f, 0.0f);
        const float speed = 50.0f;
        const float drag = 10.0f;
        bool sprint = false;
        if (!io.WantCaptureKeyboard)
        {
            if (keys[SDL_SCANCODE_W])
            {
                acceleration += camera_front;
            }
            if (keys[SDL_SCANCODE_A])
            {
                acceleration -= camera_right;
            }
            if (keys[SDL_SCANCODE_S])
            {
                acceleration -= camera_front;
            }
            if (keys[SDL_SCANCODE_D])
            {
                acceleration += camera_right;
            }
            if (keys[SDL_SCANCODE_SPACE])
            {
                acceleration.y = 1.0f;
            }
            if (keys[SDL_SCANCODE_LCTRL])
            {
                acceleration.y = -1.0f;
            }
            if (keys[SDL_SCANCODE_LSHIFT])
            {
                sprint = true;
            }
        }
        float acceleration_length = glm::length(acceleration);
        if (acceleration_length > 1.0f)
        {
            acceleration /= acceleration_length;
        }
        acceleration *= speed * (sprint ? 2.0f : 1.0f);
        acceleration -= velocity * drag;
        camera->position = 0.5f * acceleration * powf(delta_time, 2.0f) + velocity * delta_time + camera->position;
        velocity = acceleration * delta_time + velocity;
        // camera->pitch = -glm::dot(camera_front, velocity);
        camera->roll = glm::dot(camera_right, velocity);

        if (flashlight_follow)
        {
            registry.get<liminal::transform>(flashlight).position = camera->position;
            registry.get<liminal::transform>(flashlight).rotation = glm::mix(
                registry.get<liminal::transform>(flashlight).rotation,
                camera_front,
                30.0f * delta_time);
        }

        audio.set_listener(camera->position, camera_front, glm::vec3(0.0f, 1.0f, 0.0f));
        registry.get<liminal::audio_source>(ambience).set_position(camera->position);
        registry.get<liminal::audio_source>(weapon).set_position(camera->position);

        if (!io.WantCaptureMouse)
        {
            if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT))
            {
                if (!registry.get<liminal::audio_source>(weapon).is_playing())
                {
                    registry.get<liminal::audio_source>(weapon).play(shoot_sound);
                }
            }

            if (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT))
            {
                if (!registry.get<liminal::audio_source>(bounce).is_playing())
                {
                    registry.get<liminal::audio_source>(bounce).play(bounce_sound);
                }
            }
        }

        // update scripts
        for (auto [entity, script] : registry.view<liminal::script>().each())
        {
            script.update(delta_time);
        }

        // update physics
        world->stepSimulation(delta_time);

        // update animations
        for (auto [entity, mesh_renderer] : registry.view<liminal::mesh_renderer>().each())
        {
            if (mesh_renderer.model->has_animations())
            {
                mesh_renderer.model->update_bone_transformations(0, current_time);
            }
        }

        // setup window for rendering
        platform.begin_render();

        // main rendering
        renderer.render(registry, current_time, delta_time);

        // DEBUG: render demo window
        if (edit_mode)
        {
            ImGui::ShowDemoWindow();
        }

        // render console
        if (console_open)
        {
            ImGui::Begin("Console", &console_open);

            static std::vector<std::string> messages;

            // TODO: command arguments
            char command[256] = {};
            if (ImGui::InputText("Input", command, sizeof(command), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                if (strcmp(command, "help") == 0)
                {
                    messages.push_back("TODO: help");
                }
                else if (strcmp(command, "quit") == 0)
                {
                    quit = true;
                }
                else if (strcmp(command, "wireframe") == 0)
                {
                    renderer.wireframe = !renderer.wireframe;
                    messages.push_back("Wireframe " + renderer.wireframe ? "on" : "off");
                }
                else if (strcmp(command, "greyscale") == 0)
                {
                    renderer.greyscale = !renderer.greyscale;
                    messages.push_back("Wireframe " + renderer.wireframe ? "on" : "off");
                }
                else
                {
                    messages.push_back("Unknown command");
                }
            }

            ImGui::BeginChild("");
            for (auto &message : messages)
            {
                ImGui::Text(message.c_str());
            }
            ImGui::EndChild();

            ImGui::End();
        }

        // done rendering
        platform.end_render();
    }

    return 0;
}
