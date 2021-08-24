#include <bullet/btBulletDynamicsCommon.h>
#include <cxxopts.hpp>
#include <entt/entt.hpp>
#include <iostream>
#include <imgui.h>
#include <SDL2/SDL.h>
#include <sol/sol.hpp>

#include "components/mesh_renderer.hpp"
#include "components/transform.hpp"
#include "audio.hpp"
#include "directional_light.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "platform.hpp"
#include "point_light.hpp"
#include "renderer.hpp"
#include "skybox.hpp"
#include "sound.hpp"
#include "source.hpp"
#include "spot_light.hpp"
#include "sprite.hpp"
#include "terrain.hpp"
#include "water.hpp"

#define VERSION "v0.0.1"

#define WINDOW_TITLE "Liminal Engine"

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

    // lua test
    sol::state lua;
    lua.open_libraries(sol::lib::base);
    lua.set_function("PrintHelloWorld", []() -> void
                     { std::cout << "Hello, World!" << std::endl; });
    lua.set_function("GetRandomNumber", [](int mod) -> int
                     { return 4 + mod; });
    lua.script_file("assets/scripts/test.lua");

    // load assets
    // TODO: asset management
    liminal::model *backpack_model = new liminal::model("assets/models/backpack/backpack.obj");
    liminal::model *boblamp_model = new liminal::model("assets/models/boblampclean/boblampclean.md5mesh", true);
    liminal::model *dude_model = new liminal::model("assets/models/dude/model.dae", true);
    liminal::skybox *skybox = renderer.skybox = new liminal::skybox("assets/images/GCanyon_C_YumaPoint_8k.jpg");
    liminal::sound *ambient_sound = new liminal::sound("assets/audio/ambient.wav");
    liminal::sound *bounce_sound = new liminal::sound("assets/audio/bounce.wav");
    liminal::sound *shoot_sound = new liminal::sound("assets/audio/shoot.wav");

    liminal::camera *camera = renderer.camera = new liminal::camera(
        glm::vec3(0.0f, 0.0f, 3.0f),
        0.0f,
        -90.0f,
        0.0f,
        45.0f);


    auto backpack_entity = registry.create();
    registry.emplace<liminal::transform>(backpack_entity, nullptr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    registry.emplace<liminal::mesh_renderer>(backpack_entity, backpack_model);

    auto boblamp_entity = registry.create();
    registry.emplace<liminal::transform>(boblamp_entity, nullptr, glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.05f, 0.05f, 0.05f));
    registry.emplace<liminal::mesh_renderer>(boblamp_entity, boblamp_model);

    auto dude_entity = registry.create();
    registry.emplace<liminal::transform>(dude_entity, nullptr, glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    registry.emplace<liminal::mesh_renderer>(dude_entity, dude_model);

    // const float sun_intensity = 10.0f;
    // liminal::directional_light *sun = new liminal::directional_light(
    //     glm::vec3(0.352286f, -0.547564f, -0.758992f),
    //     glm::vec3(1.0f, 1.0f, 1.0f) * sun_intensity,
    //     4096);

    // const float light_intensity = 10.0f;
    // liminal::point_light *red_light = new liminal::point_light(
    //     glm::vec3(2.0f, 0.0f, 2.0f),
    //     glm::vec3(1.0f, 0.0f, 0.0f) * light_intensity,
    //     512);
    // liminal::point_light *yellow_light = new liminal::point_light(
    //     glm::vec3(-2.0f, 0.0f, -2.0f),
    //     glm::vec3(1.0f, 1.0f, 0.0f) * light_intensity,
    //     512);
    // liminal::point_light *green_light = new liminal::point_light(
    //     glm::vec3(2.0f, 0.0f, -2.0f),
    //     glm::vec3(0.0f, 1.0f, 0.0f) * light_intensity,
    //     512);
    // liminal::point_light *blue_light = new liminal::point_light(
    //     glm::vec3(-2.0f, 0.0f, 2.0f),
    //     glm::vec3(0.0f, 0.0f, 1.0f) * light_intensity,
    //     512);

    // const float flashlight_intensity = 20.0f;
    // liminal::spot_light *flashlight = new liminal::spot_light(
    //     glm::vec3(0.0f, 0.0f, 0.0f),
    //     glm::vec3(0.0f, 0.0f, 0.0f),
    //     glm::vec3(1.0f, 1.0f, 1.0f) * flashlight_intensity,
    //     cosf(glm::radians(12.5f)),
    //     cosf(glm::radians(15.0f)),
    //     1024);

    // liminal::water *water = new liminal::water(
    //     glm::vec3(0.0f, -2.0f, 0.0f),
    //     100.0f);

    // liminal::terrain *terrain = new liminal::terrain(
    //     "assets/images/heightmap.png",
    //     glm::vec3(0.0f, 0.0f, 0.0f),
    //     100.0f,
    //     10.0f);
    // world->addRigidBody(terrain->rigidbody);

    liminal::source *ambient_source = new liminal::source(glm::vec3(0.0f, 0.0f, 0.0f));
    ambient_source->set_loop(true);
    ambient_source->set_gain(0.25f);
    // ambient_source->play(ambient_sound);
    liminal::source *bounce_source = new liminal::source(glm::vec3(0.0f, 0.0f, 0.0f));
    liminal::source *shoot_source = new liminal::source(glm::vec3(0.0f, 0.0f, 0.0f));

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

        // static float angle = 0.0f;
        // const float pi = 3.14159f;
        // const float distance = 6.0f;
        // angle += 0.5f * delta_time;
        // if (angle > 2 * pi)
        // {
        //     angle = 0;
        // }
        // red_light->position.x = distance * sinf(angle);
        // red_light->position.z = distance * cosf(angle);
        // yellow_light->position.x = distance * sinf(angle + pi / 2);
        // yellow_light->position.z = distance * cosf(angle + pi / 2);
        // green_light->position.x = distance * sinf(angle + pi);
        // green_light->position.z = distance * cosf(angle + pi);
        // blue_light->position.x = distance * sinf(angle + 3 * pi / 2);
        // blue_light->position.z = distance * cosf(angle + 3 * pi / 2);

        // if (flashlight_follow)
        // {
        //     flashlight->position = camera->position;
        //     flashlight->direction = glm::mix(flashlight->direction, camera_front, 30.0f * delta_time);
        // }

        audio.set_listener(camera->position, camera_front, glm::vec3(0.0f, 1.0f, 0.0f));
        ambient_source->set_position(camera->position);
        shoot_source->set_position(camera->position);

        if (!io.WantCaptureMouse)
        {
            if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT))
            {
                if (!shoot_source->is_playing())
                {
                    shoot_source->play(shoot_sound);
                }
            }

            if (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT))
            {
                if (!bounce_source->is_playing())
                {
                    bounce_source->play(bounce_sound);
                }
            }
        }

        // update physics
        world->stepSimulation(delta_time);

        // update animations
        auto view = registry.view<liminal::mesh_renderer>();
        for (auto [entity, mesh_renderer] : view.each())
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
