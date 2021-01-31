#include <bullet/btBulletDynamicsCommon.h>
#include <cxxopts.hpp>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>

#include "audio.hpp"
#include "directional_light.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "object.hpp"
#include "point_light.hpp"
#include "renderer.hpp"
#include "skybox.hpp"
#include "sound.hpp"
#include "source.hpp"
#include "spot_light.hpp"
#include "sprite.hpp"
#include "terrain.hpp"
#include "water.hpp"

#define SDL_FLAGS (SDL_INIT_AUDIO | SDL_INIT_VIDEO)
#define IMG_FLAGS (IMG_INIT_JPG | IMG_INIT_PNG)
#define MIX_FLAGS (0)

#define VERSION "v0.0.1"

#define WINDOW_TITLE "Project Kilonova"

int main(int argc, char *argv[])
{
    int window_width;
    int window_height;
    int render_scale;

    try
    {
        cxxopts::Options options("pk");

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

    if (SDL_Init(SDL_FLAGS) != 0)
    {
        std::cerr << "Error: Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (IMG_Init(IMG_FLAGS) != IMG_FLAGS)
    {
        std::cerr << "Error: Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        return 1;
    }

    if (Mix_Init(MIX_FLAGS) != MIX_FLAGS)
    {
        std::cerr << "Error: Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        std::cerr << "Error: Failed to create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetSwapInterval(0);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context)
    {
        std::cerr << "Error: Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        return 1;
    }

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        std::cerr << "Error: Failed to initialize GLEW: " << glewGetErrorString(error) << std::endl;
        return 1;
    }

    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 460");
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = "assets/imgui.ini";

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) != 0)
    {
        std::cerr << "Error: Failed to initialize the mixer API: " << Mix_GetError() << std::endl;
        return 1;
    }

    pk::renderer renderer(
        window_width, window_height, render_scale,
        window_width, window_height,
        window_width, window_height);
    pk::audio audio;

    btDefaultCollisionConfiguration *collision_configuration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher *dispatcher = new btCollisionDispatcher(collision_configuration);
    btBroadphaseInterface *overlapping_pair_cache = new btDbvtBroadphase();
    btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver();
    btDiscreteDynamicsWorld *world = new btDiscreteDynamicsWorld(dispatcher, overlapping_pair_cache, solver, collision_configuration);

    world->setGravity(btVector3(0.0f, -9.8f, 0.0f));

    pk::camera *camera = new pk::camera(
        glm::vec3(0.0f, 0.0f, 3.0f),
        0.0f,
        -90.0f,
        0.0f,
        45.0f);

    // pk::skybox *skybox = nullptr;
    // pk::skybox *skybox = new pk::skybox("assets/images/Circus_Backstage_8k.jpg");
    pk::skybox *skybox = new pk::skybox("assets/images/GCanyon_C_YumaPoint_8k.jpg");

    pk::model *backpack_model = new pk::model("assets/models/backpack/backpack.obj");
    pk::object *backpack = new pk::object(
        backpack_model,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        1.0f);
    // world->addRigidBody(backpack->rigidbody);

    pk::model *cube_model = new pk::model("assets/models/cube/cube.obj");
    pk::object *cube = new pk::object(
        cube_model,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        1.0f);
    // world->addRigidBody(cube->rigidbody);

    const float sun_intensity = 10.0f;
    pk::directional_light *sun = new pk::directional_light(
        glm::vec3(0.352286f, -0.547564f, -0.758992f),
        glm::vec3(1.0f, 1.0f, 1.0f) * sun_intensity,
        4096);

    const float light_intensity = 10.0f;
    pk::point_light *red_light = new pk::point_light(
        glm::vec3(2.0f, 0.0f, 2.0f),
        glm::vec3(1.0f, 0.0f, 0.0f) * light_intensity,
        512);
    pk::point_light *yellow_light = new pk::point_light(
        glm::vec3(-2.0f, 0.0f, -2.0f),
        glm::vec3(1.0f, 1.0f, 0.0f) * light_intensity,
        512);
    pk::point_light *green_light = new pk::point_light(
        glm::vec3(2.0f, 0.0f, -2.0f),
        glm::vec3(0.0f, 1.0f, 0.0f) * light_intensity,
        512);
    pk::point_light *blue_light = new pk::point_light(
        glm::vec3(-2.0f, 0.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 1.0f) * light_intensity,
        512);

    const float flashlight_intensity = 20.0f;
    pk::spot_light *flashlight = new pk::spot_light(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f) * flashlight_intensity,
        cosf(glm::radians(12.5f)),
        cosf(glm::radians(15.0f)),
        1024);

    pk::water *water = new pk::water(
        glm::vec3(0.0f, -2.0f, 0.0f),
        glm::vec2(100.0f, 100.0f));

    pk::terrain *terrain = new pk::terrain(glm::vec3(400.0f, 0.0f, 400.0f), "assets/images/heightmap.png");
    // world->addRigidBody(terrain->rigidbody);

    pk::sound *ambient_sound = new pk::sound("assets/audio/ambient.wav");
    pk::sound *bounce_sound = new pk::sound("assets/audio/bounce.wav");
    pk::sound *shoot_sound = new pk::sound("assets/audio/shoot.wav");

    pk::source *ambient_source = new pk::source(glm::vec3(0.0f, 0.0f, 0.0f));
    ambient_source->set_loop(true);
    ambient_source->set_gain(0.25f);
    // ambient_source->play(ambient_sound);
    pk::source *bounce_source = new pk::source(glm::vec3(0.0f, 0.0f, 0.0f));
    pk::source *shoot_source = new pk::source(glm::vec3(0.0f, 0.0f, 0.0f));

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
            ImGui_ImplSDL2_ProcessEvent(&event);

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
                    SDL_SetWindowSize(window, window_width, window_height);
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
                            unsigned int flags = SDL_GetWindowFlags(window);
                            if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
                            {
                                SDL_SetWindowFullscreen(window, 0);
                            }
                            else
                            {
                                SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                            }
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

        SDL_GL_MakeCurrent(window, context);

        renderer.wireframe = wireframe;
        renderer.camera = camera;
        renderer.skybox = skybox;
        renderer.objects.push_back(backpack);
        // renderer->objects.push_back(cube);
        renderer.directional_lights.push_back(sun);
        renderer.point_lights.push_back(red_light);
        renderer.point_lights.push_back(yellow_light);
        renderer.point_lights.push_back(green_light);
        renderer.point_lights.push_back(blue_light);
        if (flashlight_on)
        {
            renderer.spot_lights.push_back(flashlight);
        }
        renderer.terrains.push_back(terrain);
        renderer.waters.push_back(water);
        renderer.flush(current_time, delta_time);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

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

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    delete world;
    delete solver;
    delete overlapping_pair_cache;
    delete dispatcher;
    delete collision_configuration;

    delete camera;

    delete skybox;

    delete backpack_model;
    delete backpack;

    delete cube_model;
    delete cube;

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

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    Mix_CloseAudio();

    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(context);

    IMG_Quit();
    Mix_Quit();

    SDL_Quit();

    return 0;
}
