#include "core/engine.hpp"

#include <AL/al.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <cxxopts.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <imgui.h>
#include <SDL2/SDL.h>
#include <string>

#include "audio/sound.hpp"
#include "components/audio_source.hpp"
#include "components/directional_light.hpp"
#include "components/mesh_renderer.hpp"
#include "components/script.hpp"
#include "components/spot_light.hpp"
#include "components/sprite.hpp"
#include "components/terrain.hpp"
#include "components/transform.hpp"
#include "core/app.hpp"
#include "core/assets.hpp"
#include "core/platform.hpp"
#include "core/scene.hpp"
#include "graphics/camera.hpp"
#include "graphics/model.hpp"
#include "graphics/renderer.hpp"
#include "graphics/skybox.hpp"
#include "graphics/texture.hpp"

constexpr const char *exe_name = "liminal";
constexpr const char *version_string = "v0.0.1";
constexpr const char *window_title = "Liminal Engine";

void liminal::engine::run(int argc, char *argv[])
{
    // parse command line options
    int window_width;
    int window_height;
    int render_scale;

    try
    {
        cxxopts::Options options(exe_name);

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
            return;
        }

        render_scale = glm::clamp(result["scale"].as<float>(), 0.1f, 1.0f);

        if (result.count("version"))
        {
            std::cout << version_string << std::endl;
            return;
        }

        window_width = result["width"].as<int>();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return;
    }

    // init subsystems
    liminal::platform platform(window_title, window_width, window_height);
    liminal::renderer renderer(
        window_width, window_height, render_scale,
        4096, 512, 1024,
        window_width, window_height,
        window_width, window_height);

    // get imgui reference
    ImGuiIO &io = ImGui::GetIO();

    // create client app
    auto app = liminal::create_app(argc, argv);

    // load assets
    // TODO: asset management
    liminal::sound ambient_sound("assets/audio/ambient.wav");
    liminal::sound bounce_sound("assets/audio/bounce.wav");
    liminal::sound shoot_sound("assets/audio/shoot.wav");
    liminal::texture grass_texture("assets/images/grass_sprite.png");

    // create scene
    // TODO: non-hardcoded way to determine first scene to load
    const std::string &scene_filename = "assets/scenes/demo.json";
    auto scene = new liminal::scene(scene_filename);

    // TODO: these entities should come from the JSON file
    const float flashlight_intensity = 20.0f;
    auto flashlight_entity = scene->registry.create();
    scene->registry.emplace<liminal::transform>(flashlight_entity, "Flashlight", nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f);
    scene->registry.emplace<liminal::spot_light>(flashlight_entity, glm::vec3(1.0f, 1.0f, 1.0f) * flashlight_intensity, cosf(glm::radians(12.5f)), cosf(glm::radians(15.0f)));

    auto ambience_entity = scene->registry.create();
    scene->registry.emplace<liminal::transform>(ambience_entity, "Ambience", nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f);
    scene->registry.emplace<liminal::audio_source>(ambience_entity);
    scene->registry.get<liminal::audio_source>(ambience_entity).set_loop(true);
    scene->registry.get<liminal::audio_source>(ambience_entity).set_gain(0.25f);
    // scene->registry.get<liminal::audio_source>(ambience_entity).play(ambient_sound);

    auto bounce_entity = scene->registry.create();
    scene->registry.emplace<liminal::transform>(bounce_entity, "Bounce sound", nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f);
    scene->registry.emplace<liminal::audio_source>(bounce_entity);

    auto weapon_entity = scene->registry.create();
    scene->registry.emplace<liminal::transform>(weapon_entity, "Weapon", nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f);
    scene->registry.emplace<liminal::audio_source>(weapon_entity);

    auto ui_entity = scene->registry.create();
    // scene->registry.emplace<liminal::sprite>(ui_entity, &grass_texture, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(1.0f, 1.0f));}

    // TODO: camera should be an entity as well
    auto camera = scene->camera = new liminal::camera(
        glm::vec3(0.0f, 0.0f, 3.0f),
        0.0f,
        0.0f,
        0.0f,
        45.0f);

    unsigned int current_time = 0;
    float time_scale = 1.0f;
    bool console_open = false;

    // TODO: script driven
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

        // TODO: this block should be script driven
        {
            if (flashlight_follow)
            {
                scene->registry.get<liminal::transform>(flashlight_entity).position = camera->position;
                scene->registry.get<liminal::transform>(flashlight_entity).rotation = glm::mix(
                    scene->registry.get<liminal::transform>(flashlight_entity).rotation,
                    camera_front,
                    30.0f * delta_time);
            }
            if (flashlight_on)
            {
                scene->registry.get<liminal::spot_light>(flashlight_entity).color = glm::vec3(1.0f, 1.0f, 1.0f) * flashlight_intensity;
            }
            else
            {
                scene->registry.get<liminal::spot_light>(flashlight_entity).color = glm::vec3(0.0f, 0.0f, 0.0f);
            }

            // TODO: this should be a component
            // and restrict to only having one audio listener component active at any time
            {
                static glm::vec3 last_camera_position;

                alListenerfv(AL_POSITION, glm::value_ptr(camera->position));

                glm::vec3 velocity = last_camera_position - camera->position;
                alListenerfv(AL_VELOCITY, glm::value_ptr(velocity));

                float orientation[] = {
                    camera_front.x, camera_front.y, camera_front.z,
                    0.0f, 1.0f, 0.0f};
                alListenerfv(AL_ORIENTATION, orientation);

                last_camera_position = camera->position;
            }

            scene->registry.get<liminal::audio_source>(ambience_entity).set_position(camera->position);
            scene->registry.get<liminal::audio_source>(weapon_entity).set_position(camera->position);

            if (!io.WantCaptureMouse)
            {
                if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT))
                {
                    if (!scene->registry.get<liminal::audio_source>(weapon_entity).is_playing())
                    {
                        scene->registry.get<liminal::audio_source>(weapon_entity).play(shoot_sound);
                    }
                }

                if (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT))
                {
                    if (!scene->registry.get<liminal::audio_source>(bounce_entity).is_playing())
                    {
                        scene->registry.get<liminal::audio_source>(bounce_entity).play(bounce_sound);
                    }
                }
            }
        }

        // start of frame
        platform.begin_frame();

        // update client app
        app->update(delta_time);

        // update scripts
        for (auto [entity, script] : scene->registry.view<liminal::script>().each())
        {
            script.update(delta_time);
        }

        // update physics
        scene->world->stepSimulation(delta_time);

        // update animations
        for (auto [entity, mesh_renderer] : scene->registry.view<liminal::mesh_renderer>().each())
        {
            if (mesh_renderer.model->has_animations())
            {
                mesh_renderer.model->update_bone_transformations(0, current_time);
            }
        }

        // render everything
        renderer.render(*scene, current_time, delta_time);

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
                    messages.push_back("Greyscale " + renderer.greyscale ? "on" : "off");
                }
                else if (strcmp(command, "reload") == 0)
                {
                    delete scene;
                    scene = new liminal::scene(scene_filename);
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

        // end of frame
        platform.end_frame();
    }

    delete scene;

    delete app;
}
