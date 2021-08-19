#include <bullet/btBulletDynamicsCommon.h>
#include <cxxopts.hpp>
#include <iostream>
#include <imgui.h>
#include <SDL2/SDL.h>
#include <sol/sol.hpp>

#include "audio.hpp"
#include "directional_light.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "object.hpp"
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

    liminal::platform platform(WINDOW_TITLE, window_width, window_height);
    liminal::renderer renderer(
        window_width, window_height, render_scale,
        window_width, window_height,
        window_width, window_height);
    liminal::audio audio;

    ImGuiIO &io = ImGui::GetIO();

    btDefaultCollisionConfiguration *collision_configuration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher *dispatcher = new btCollisionDispatcher(collision_configuration);
    btBroadphaseInterface *overlapping_pair_cache = new btDbvtBroadphase();
    btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver();
    btDiscreteDynamicsWorld *world = new btDiscreteDynamicsWorld(dispatcher, overlapping_pair_cache, solver, collision_configuration);

    world->setGravity(btVector3(0.0f, -9.8f, 0.0f));

    sol::state lua;
    lua.open_libraries(sol::lib::base);
    lua.set_function("PrintHelloWorld", []() -> void
                     { std::cout << "Hello, World!" << std::endl; });
    lua.set_function("GetRandomNumber", [](int mod) -> int
                     { return 4 + mod; });
    lua.script_file("assets/scripts/test.lua");

    liminal::camera *camera = new liminal::camera(
        glm::vec3(0.0f, 0.0f, 3.0f),
        0.0f,
        -90.0f,
        0.0f,
        45.0f);

    // liminal::skybox *skybox = nullptr;
    // liminal::skybox *skybox = new liminal::skybox("assets/images/Circus_Backstage_8k.jpg");
    liminal::skybox *skybox = new liminal::skybox("assets/images/GCanyon_C_YumaPoint_8k.jpg");

    // liminal::model *model = new liminal::model("assets/models/cube/cube.obj");
    liminal::model *model = new liminal::model("assets/models/backpack/backpack.obj");
    liminal::object *object = new liminal::object(
        model,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        1.0f);
    // world->addRigidBody(object->rigidbody);

    liminal::model *animated_model = new liminal::model("assets/models/boblampclean/boblampclean.md5mesh", true);
    animated_model->set_animation(0);
    liminal::object *animated_object = new liminal::object(
        animated_model,
        glm::vec3(5.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, -1.57f, 0.0f),
        glm::vec3(0.05f, 0.05f, 0.05f),
        1.0f);
    // world->addRigidBody(animated_object->rigidbody);

    liminal::model *animated_model2 = new liminal::model("assets/models/dude/model.dae", true);
    animated_model2->set_animation(0);
    liminal::object *animated_object2 = new liminal::object(
        animated_model2,
        glm::vec3(-5.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, -1.57f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        1.0f);
    // world->addRigidBody(animated_object2->rigidbody);

    const float sun_intensity = 10.0f;
    liminal::directional_light *sun = new liminal::directional_light(
        glm::vec3(0.352286f, -0.547564f, -0.758992f),
        glm::vec3(1.0f, 1.0f, 1.0f) * sun_intensity,
        4096);

    const float light_intensity = 10.0f;
    liminal::point_light *red_light = new liminal::point_light(
        glm::vec3(2.0f, 0.0f, 2.0f),
        glm::vec3(1.0f, 0.0f, 0.0f) * light_intensity,
        512);
    liminal::point_light *yellow_light = new liminal::point_light(
        glm::vec3(-2.0f, 0.0f, -2.0f),
        glm::vec3(1.0f, 1.0f, 0.0f) * light_intensity,
        512);
    liminal::point_light *green_light = new liminal::point_light(
        glm::vec3(2.0f, 0.0f, -2.0f),
        glm::vec3(0.0f, 1.0f, 0.0f) * light_intensity,
        512);
    liminal::point_light *blue_light = new liminal::point_light(
        glm::vec3(-2.0f, 0.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 1.0f) * light_intensity,
        512);

    const float flashlight_intensity = 20.0f;
    liminal::spot_light *flashlight = new liminal::spot_light(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f) * flashlight_intensity,
        cosf(glm::radians(12.5f)),
        cosf(glm::radians(15.0f)),
        1024);

    liminal::water *water = new liminal::water(
        glm::vec3(0.0f, -2.0f, 0.0f),
        100.0f);

    liminal::terrain *terrain = new liminal::terrain(
        "assets/images/heightmap.png",
        glm::vec3(0.0f, 0.0f, 0.0f),
        100.0f,
        10.0f);
    // world->addRigidBody(terrain->rigidbody);

    liminal::sound *ambient_sound = new liminal::sound("assets/audio/ambient.wav");
    liminal::sound *bounce_sound = new liminal::sound("assets/audio/bounce.wav");
    liminal::sound *shoot_sound = new liminal::sound("assets/audio/shoot.wav");

    liminal::source *ambient_source = new liminal::source(glm::vec3(0.0f, 0.0f, 0.0f));
    ambient_source->set_loop(true);
    ambient_source->set_gain(0.25f);
    // ambient_source->play(ambient_sound);
    liminal::source *bounce_source = new liminal::source(glm::vec3(0.0f, 0.0f, 0.0f));
    liminal::source *shoot_source = new liminal::source(glm::vec3(0.0f, 0.0f, 0.0f));

    unsigned int current_time = 0;
    float time_scale = 1.0f;
    bool console_open = false;
    bool wireframe = false;
    bool edit_mode = false;
    bool lock_cursor = true;
    bool flashlight_on = true;
    bool flashlight_follow = true;

    bool quit = false;
    while (!quit)
    {
        unsigned int previous_time = current_time;
        current_time = SDL_GetTicks();
        float delta_time = ((current_time - previous_time) / 1000.0f) * time_scale;

        int num_keys;
        const unsigned char *keys = SDL_GetKeyboardState(&num_keys);
        int mouse_x, mouse_y;
        unsigned int mouse = SDL_GetMouseState(&mouse_x, &mouse_y);

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
                        lock_cursor = !lock_cursor;
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

        SDL_SetRelativeMouseMode((SDL_bool)lock_cursor);

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

        static float angle = 0.0f;
        const float pi = 3.14159f;
        const float distance = 6.0f;
        angle += 0.5f * delta_time;
        if (angle > 2 * pi)
        {
            angle = 0;
        }
        red_light->position.x = distance * sinf(angle);
        red_light->position.z = distance * cosf(angle);
        yellow_light->position.x = distance * sinf(angle + pi / 2);
        yellow_light->position.z = distance * cosf(angle + pi / 2);
        green_light->position.x = distance * sinf(angle + pi);
        green_light->position.z = distance * cosf(angle + pi);
        blue_light->position.x = distance * sinf(angle + 3 * pi / 2);
        blue_light->position.z = distance * cosf(angle + 3 * pi / 2);

        if (flashlight_follow)
        {
            flashlight->position = camera->position;
            flashlight->direction = glm::mix(flashlight->direction, camera_front, 30.0f * delta_time);
        }

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

        world->stepSimulation(delta_time);

        platform.begin_render();

        renderer.wireframe = wireframe;
        renderer.camera = camera;
        renderer.skybox = skybox;
        renderer.objects.push_back(object);
        renderer.objects.push_back(animated_object);
        renderer.objects.push_back(animated_object2);
        renderer.directional_lights.push_back(sun);
        renderer.point_lights.push_back(red_light);
        renderer.point_lights.push_back(yellow_light);
        renderer.point_lights.push_back(green_light);
        renderer.point_lights.push_back(blue_light);
        if (flashlight_on)
        {
            renderer.spot_lights.push_back(flashlight);
        }
        // renderer.terrains.push_back(terrain);
        renderer.waters.push_back(water);
        renderer.flush(current_time, delta_time);

        if (edit_mode)
        {
            ImGui::ShowDemoWindow();
        }

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
                    wireframe = !wireframe;
                    messages.push_back("Wireframe " + wireframe ? "on" : "off");
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

        platform.end_render();
    }

    delete world;
    delete solver;
    delete overlapping_pair_cache;
    delete dispatcher;
    delete collision_configuration;

    delete camera;

    delete skybox;

    delete model;
    delete object;

    delete animated_model;
    delete animated_object;

    delete animated_model2;
    delete animated_object2;

    delete sun;

    delete red_light;
    delete yellow_light;
    delete green_light;
    delete blue_light;

    delete flashlight;

    delete water;

    delete terrain;

    delete ambient_source;
    delete bounce_source;
    delete shoot_source;

    delete ambient_sound;
    delete bounce_sound;
    delete shoot_sound;

    return 0;
}
