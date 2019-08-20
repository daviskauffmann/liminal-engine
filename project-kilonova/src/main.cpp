#include <iostream>

#include <glm/glm.hpp>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_ttf.h>

#include "audio.hpp"
#include "camera.hpp"
#include "cubemap.hpp"
#include "directional_light.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "point_light.hpp"
#include "program.hpp"
#include "renderer.hpp"
#include "sound.hpp"
#include "source.hpp"
#include "spot_light.hpp"
#include "sprite.hpp"
#include "sun.hpp"
#include "texture.hpp"
#include "vertex.hpp"
#include "water.hpp"

constexpr auto window_title = "Project Kilonova";
constexpr auto window_width = 1280;
constexpr auto window_height = 720;

constexpr auto render_scale = 1.0f;

constexpr auto shadow_width = 4096;
constexpr auto shadow_height = 4096;

constexpr auto fps_cap = 300;
constexpr auto frame_delay = 1000 / fps_cap;

int main(int argc, char *argv[])
{
    // parse command line arguments
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            std::cout << "Options:" << std::endl;
            std::cout << "  -h, --help\tPrint this message" << std::endl;
        }
    }

    // init SDL
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    Mix_Init(0);
    TTF_Init();
    SDLNet_Init();

    // create window
    SDL_Window *window = SDL_CreateWindow(
        window_title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    // create OpenGL context
    SDL_GLContext context = SDL_GL_CreateContext(window);

    // setup audio
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);

    // setup renderer and audio
    pk::renderer renderer(window_width, window_height, render_scale, shadow_width, shadow_height);
    pk::audio audio;

    // create meshes
    pk::mesh quad_mesh(
        {pk::vertex(glm::vec3(+1.0f, +1.0f, +0.0f), glm::vec3(+0.0f, +1.0f, +0.0f), glm::vec2(+1.0f, +1.0f)),
         pk::vertex(glm::vec3(+1.0f, -1.0f, +0.0f), glm::vec3(+0.0f, +1.0f, +0.0f), glm::vec2(+1.0f, +0.0f)),
         pk::vertex(glm::vec3(-1.0f, -1.0f, +0.0f), glm::vec3(+0.0f, +1.0f, +0.0f), glm::vec2(+0.0f, +0.0f)),
         pk::vertex(glm::vec3(-1.0f, +1.0f, +0.0f), glm::vec3(+0.0f, +1.0f, +0.0f), glm::vec2(+0.0f, +1.0f))},
        {0, 1, 3,
         1, 2, 3});
    pk::mesh cube_mesh(
        {pk::vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(+0.0f, +0.0f, -1.0f), glm::vec2(+0.0f, +0.0f)),
         pk::vertex(glm::vec3(+1.0f, +1.0f, -1.0f), glm::vec3(+0.0f, +0.0f, -1.0f), glm::vec2(+1.0f, +1.0f)),
         pk::vertex(glm::vec3(+1.0f, -1.0f, -1.0f), glm::vec3(+0.0f, +0.0f, -1.0f), glm::vec2(+1.0f, +0.0f)),
         pk::vertex(glm::vec3(+1.0f, +1.0f, -1.0f), glm::vec3(+0.0f, +0.0f, -1.0f), glm::vec2(+1.0f, +1.0f)),
         pk::vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(+0.0f, +0.0f, -1.0f), glm::vec2(+0.0f, +0.0f)),
         pk::vertex(glm::vec3(-1.0f, +1.0f, -1.0f), glm::vec3(+0.0f, +0.0f, -1.0f), glm::vec2(+0.0f, +1.0f)),
         pk::vertex(glm::vec3(-1.0f, -1.0f, +1.0f), glm::vec3(+0.0f, +0.0f, +1.0f), glm::vec2(+0.0f, +0.0f)),
         pk::vertex(glm::vec3(+1.0f, -1.0f, +1.0f), glm::vec3(+0.0f, +0.0f, +1.0f), glm::vec2(+1.0f, +0.0f)),
         pk::vertex(glm::vec3(+1.0f, +1.0f, +1.0f), glm::vec3(+0.0f, +0.0f, +1.0f), glm::vec2(+1.0f, +1.0f)),
         pk::vertex(glm::vec3(+1.0f, +1.0f, +1.0f), glm::vec3(+0.0f, +0.0f, +1.0f), glm::vec2(+1.0f, +1.0f)),
         pk::vertex(glm::vec3(-1.0f, +1.0f, +1.0f), glm::vec3(+0.0f, +0.0f, +1.0f), glm::vec2(+0.0f, +1.0f)),
         pk::vertex(glm::vec3(-1.0f, -1.0f, +1.0f), glm::vec3(+0.0f, +0.0f, +1.0f), glm::vec2(+0.0f, +0.0f)),
         pk::vertex(glm::vec3(-1.0f, +1.0f, +1.0f), glm::vec3(-1.0f, +0.0f, +0.0f), glm::vec2(+1.0f, +0.0f)),
         pk::vertex(glm::vec3(-1.0f, +1.0f, -1.0f), glm::vec3(-1.0f, +0.0f, +0.0f), glm::vec2(+1.0f, +1.0f)),
         pk::vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, +0.0f, +0.0f), glm::vec2(+0.0f, +1.0f)),
         pk::vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, +0.0f, +0.0f), glm::vec2(+0.0f, +1.0f)),
         pk::vertex(glm::vec3(-1.0f, -1.0f, +1.0f), glm::vec3(-1.0f, +0.0f, +0.0f), glm::vec2(+0.0f, +0.0f)),
         pk::vertex(glm::vec3(-1.0f, +1.0f, +1.0f), glm::vec3(-1.0f, +0.0f, +0.0f), glm::vec2(+1.0f, +0.0f)),
         pk::vertex(glm::vec3(+1.0f, +1.0f, +1.0f), glm::vec3(+1.0f, +0.0f, +0.0f), glm::vec2(+1.0f, +0.0f)),
         pk::vertex(glm::vec3(+1.0f, -1.0f, -1.0f), glm::vec3(+1.0f, +0.0f, +0.0f), glm::vec2(+0.0f, +1.0f)),
         pk::vertex(glm::vec3(+1.0f, +1.0f, -1.0f), glm::vec3(+1.0f, +0.0f, +0.0f), glm::vec2(+1.0f, +1.0f)),
         pk::vertex(glm::vec3(+1.0f, -1.0f, -1.0f), glm::vec3(+1.0f, +0.0f, +0.0f), glm::vec2(+0.0f, +1.0f)),
         pk::vertex(glm::vec3(+1.0f, +1.0f, +1.0f), glm::vec3(+1.0f, +0.0f, +0.0f), glm::vec2(+1.0f, +0.0f)),
         pk::vertex(glm::vec3(+1.0f, -1.0f, +1.0f), glm::vec3(+1.0f, +0.0f, +0.0f), glm::vec2(+0.0f, +0.0f)),
         pk::vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(+0.0f, -1.0f, +0.0f), glm::vec2(+0.0f, +1.0f)),
         pk::vertex(glm::vec3(+1.0f, -1.0f, -1.0f), glm::vec3(+0.0f, -1.0f, +0.0f), glm::vec2(+1.0f, +1.0f)),
         pk::vertex(glm::vec3(+1.0f, -1.0f, +1.0f), glm::vec3(+0.0f, -1.0f, +0.0f), glm::vec2(+1.0f, +0.0f)),
         pk::vertex(glm::vec3(+1.0f, -1.0f, +1.0f), glm::vec3(+0.0f, -1.0f, +0.0f), glm::vec2(+1.0f, +0.0f)),
         pk::vertex(glm::vec3(-1.0f, -1.0f, +1.0f), glm::vec3(+0.0f, -1.0f, +0.0f), glm::vec2(+0.0f, +0.0f)),
         pk::vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(+0.0f, -1.0f, +0.0f), glm::vec2(+0.0f, +1.0f)),
         pk::vertex(glm::vec3(-1.0f, +1.0f, -1.0f), glm::vec3(+0.0f, +1.0f, +0.0f), glm::vec2(+0.0f, +1.0f)),
         pk::vertex(glm::vec3(+1.0f, +1.0f, +1.0f), glm::vec3(+0.0f, +1.0f, +0.0f), glm::vec2(+1.0f, +0.0f)),
         pk::vertex(glm::vec3(+1.0f, +1.0f, -1.0f), glm::vec3(+0.0f, +1.0f, +0.0f), glm::vec2(+1.0f, +1.0f)),
         pk::vertex(glm::vec3(+1.0f, +1.0f, +1.0f), glm::vec3(+0.0f, +1.0f, +0.0f), glm::vec2(+1.0f, +0.0f)),
         pk::vertex(glm::vec3(-1.0f, +1.0f, -1.0f), glm::vec3(+0.0f, +1.0f, +0.0f), glm::vec2(+0.0f, +1.0f)),
         pk::vertex(glm::vec3(-1.0f, +1.0f, +1.0f), glm::vec3(+0.0f, +1.0f, +0.0f), glm::vec2(+0.0f, +0.0f))},
        {0, 1, 2,
         3, 4, 5,
         6, 7, 8,
         9, 10, 11,
         12, 13, 14,
         15, 16, 17,
         18, 19, 20,
         21, 22, 23,
         24, 25, 26,
         27, 28, 29,
         30, 31, 32,
         33, 34, 35});

    // create textures
    pk::texture default_texture("assets/images/default.png");
    pk::texture box_diffuse_texture("assets/images/box_diffuse.png");
    pk::texture box_specular_texture("assets/images/box_specular.png");
    pk::texture cobble_diffuse_texture("assets/images/cobble_diffuse.jpg");
    pk::texture cobble_specular_texture("assets/images/cobble_specular.jpg");
    pk::texture grass_texture("assets/images/grass.png");

    // create cubemaps
    pk::cubemap skybox_cubemap(
        {"assets/images/sky/right.jpg",
         "assets/images/sky/left.jpg",
         "assets/images/sky/top.jpg",
         "assets/images/sky/bottom.jpg",
         "assets/images/sky/front.jpg",
         "assets/images/sky/back.jpg"});

    // create sounds
    pk::sound bounce_sound("assets/audio/bounce.wav");
    pk::sound shoot_sound("assets/audio/shoot.wav");

    // create materials
    pk::material default_material(
        glm::vec3(1.0f, 1.0f, 1.0f),
        &default_texture,
        &default_texture,
        16.0f,
        nullptr,
        nullptr,
        1.0f);
    pk::material box_material(
        glm::vec3(1.0f, 1.0f, 1.0f),
        &box_diffuse_texture,
        &box_specular_texture,
        16.0f,
        nullptr,
        nullptr,
        1.0f);
    pk::material cobble_material(
        glm::vec3(1.0f, 1.0f, 1.0f),
        &cobble_diffuse_texture,
        &cobble_specular_texture,
        16.0f,
        nullptr,
        nullptr,
        1.0f);

    // create objects
    pk::object floor_object(
        &cube_mesh,
        &cobble_material,
        glm::vec3(0.0f, -4.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(10.0f, 1.0f, 10.0f));
    pk::object box_1_object(
        &cube_mesh,
        &default_material,
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));
    pk::object box_2_object(
        &cube_mesh,
        &box_material,
        glm::vec3(2.0f, -0.5f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f));
    pk::object box_3_object(
        &cube_mesh,
        &box_material,
        glm::vec3(0.0f, -0.5f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f));
    pk::object box_4_object(
        &cube_mesh,
        &box_material,
        glm::vec3(-2.0f, -0.5f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f));
    pk::object box_5_object(
        &cube_mesh,
        &box_material,
        glm::vec3(0.0f, -0.5f, -2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f));

    // create sun
    pk::sun sun(
        glm::vec3(-0.2f, -1.0f, -0.3f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        glm::vec3(0.8f, 0.8f, 0.8f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    // create point lights
    pk::point_light red_point_light(
        glm::vec3(2.0f, 0.0f, 2.0f),
        glm::vec3(0.1f, 0.0f, 0.0f),
        glm::vec3(0.8f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.09f, 0.32f));
    pk::point_light yellow_point_light(
        glm::vec3(-2.0f, 0.0f, -2.0f),
        glm::vec3(0.1f, 0.1f, 0.0f),
        glm::vec3(0.8f, 0.8f, 0.0f),
        glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 0.09f, 0.32f));
    pk::point_light green_point_light(
        glm::vec3(2.0f, 0.0f, -2.0f),
        glm::vec3(0.0f, 0.1f, 0.0f),
        glm::vec3(0.0f, 0.8f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 0.09f, 0.32f));
    pk::point_light blue_point_light(
        glm::vec3(-2.0f, 0.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.1f),
        glm::vec3(0.0f, 0.0f, 0.8f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(1.0f, 0.09f, 0.32f));

    // create spot lights
    pk::spot_light torch_spot_light(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 0.09f, 0.32f),
        cosf(glm::radians(12.5f)),
        cosf(glm::radians(15.0f)));

    // create waters
    pk::water test_water(
        glm::vec3(0.0f, -2.0f, 0.0f),
        glm::vec2(100.0f, 100.0f));

    // create sprites
    pk::sprite grass_sprite(
        glm::vec3(1.0f, 1.0f, 1.0f),
        &grass_texture,
        glm::vec2(0.0f, 0.0f),
        0.0f,
        glm::vec2(1.0f, 1.0f));

    // create camera
    pk::camera main_camera(
        glm::vec3(0.0f, 0.0f, 3.0f),
        0.0f,
        -90.0f,
        0.0f,
        45.0f);

    // create sources
    pk::source origin_source;
    pk::source camera_source;

    // game settings
    unsigned int current_time = 0;
    float fps_update_timer = 0.0f;
    bool torch = true;
    float bounce_timer = 0.0f;
    float shoot_timer = 0.0f;

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_GL_SetSwapInterval(0);

    // main loop
    bool quit = false;
    while (!quit)
    {
        // timer for fps cap
        unsigned int frame_start = SDL_GetTicks();

        // calculate time passed since last frame
        unsigned int previous_time = current_time;
        current_time = frame_start;

        // calculate delta time and fps
        float delta_time = (current_time - previous_time) / 1000.0f;
        unsigned int fps = (unsigned int)(1 / delta_time);

        // update window title
        fps_update_timer += delta_time;

        if (fps_update_timer > 0.25f)
        {
            fps_update_timer = 0.0f;

            char title[256];
            sprintf_s(title, sizeof(title), "%s - FPS: %d", window_title, fps);
            SDL_SetWindowTitle(window, title);
        }

        // get keyboard input
        int num_keys;
        const unsigned char *keys = SDL_GetKeyboardState(&num_keys);

        // get mouse input
        int mouse_x, mouse_y;
        unsigned int mouse = SDL_GetMouseState(&mouse_x, &mouse_y);

        // handle events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_F4:
                {
                    if (keys[SDL_SCANCODE_LALT])
                    {
                        quit = true;
                    }
                }
                break;
                case SDLK_f:
                {
                    torch = !torch;
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
                case SDLK_TAB:
                {
                    SDL_SetRelativeMouseMode((SDL_bool)!SDL_GetRelativeMouseMode());
                }
                break;
                }
            }
            break;
            case SDL_MOUSEMOTION:
            {
                if (SDL_GetRelativeMouseMode())
                {
                    // mouselook
                    main_camera.pitch -= event.motion.yrel * 0.1f;
                    main_camera.yaw += event.motion.xrel * 0.1f;

                    if (main_camera.pitch > 89.0f)
                    {
                        main_camera.pitch = 89.0f;
                    }
                    if (main_camera.pitch < -89.0f)
                    {
                        main_camera.pitch = -89.0f;
                    }
                }
            }
            break;
            case SDL_MOUSEWHEEL:
            {
                // zoom
                if (main_camera.fov >= 1.0f && main_camera.fov <= 90.0f)
                {
                    main_camera.fov -= event.wheel.y;
                }
                if (main_camera.fov <= 1.0f)
                {
                    main_camera.fov = 1.0f;
                }
                if (main_camera.fov >= 90.0f)
                {
                    main_camera.fov = 90.0f;
                }
            }
            break;
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
                    int width = event.window.data1;
                    int height = event.window.data2;

                    SDL_SetWindowSize(window, width, height);

                    std::cout << "Window resized to " << width << "x" << height << std::endl;
                }
                break;
                }
            }
            break;
            }
        }

        // calculate movement speed
        float speed = 5.0f * delta_time;

        // sprinting
        if (keys[SDL_SCANCODE_LSHIFT])
        {
            speed *= 2.0f;
        }

        // slow movement speed when moving diagonally
        if ((keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_D]) ||
            (keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_A]) ||
            (keys[SDL_SCANCODE_S] && keys[SDL_SCANCODE_D]) ||
            (keys[SDL_SCANCODE_S] && keys[SDL_SCANCODE_A]))
        {
            // precomputed 1 / sqrt(2)
            speed *= 0.71f;
        }

        // calculate camera vectors
        glm::vec3 main_camera_front = main_camera.calc_front();
        glm::vec3 main_camera_up = main_camera.calc_up();

        // move forward
        if (keys[SDL_SCANCODE_W])
        {
            main_camera.position += main_camera_front * speed;
        }

        // strafe left
        if (keys[SDL_SCANCODE_A])
        {
            main_camera.position -= glm::normalize(glm::cross(main_camera_front, main_camera_up)) * speed;
        }

        // move backward
        if (keys[SDL_SCANCODE_S])
        {
            main_camera.position -= main_camera_front * speed;
        }

        // strafe right
        if (keys[SDL_SCANCODE_D])
        {
            main_camera.position += glm::normalize(glm::cross(main_camera_front, main_camera_up)) * speed;
        }

        // calculate angle for rotating stuff
        float angle = current_time * 0.001f;
        float angle_sin = sinf(angle);
        float angle_cos = cosf(angle);

        // update objects
        box_1_object.rotation[0] = angle_sin;
        box_1_object.rotation[1] = angle_cos;

        // update lights
        // sun.direction[0] = angle_sin;
        // sun.direction[2] = angle_cos;

        torch_spot_light.position = main_camera.position;
        torch_spot_light.direction = main_camera_front;

        // update audio
        audio.set_listener(main_camera.position, main_camera_front, main_camera_up);

        // update sources
        camera_source.set_position(main_camera.position);

        // shooting
        shoot_timer += delta_time;

        if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            if (shoot_timer >= 0.25f)
            {
                shoot_timer = 0.0f;

                camera_source.play(&shoot_sound);
            }
        }

        // 3d audio test
        bounce_timer += delta_time;

        if (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT))
        {
            if (bounce_timer >= 0.25f)
            {
                bounce_timer = 0.0f;

                origin_source.play(&bounce_sound);
            }
        }

        // setup renderer
        renderer.add_object(&floor_object);
        renderer.add_object(&box_1_object);
        renderer.add_object(&box_2_object);
        renderer.add_object(&box_3_object);
        renderer.add_object(&box_4_object);
        renderer.add_object(&box_5_object);

        renderer.set_sun(&sun);

        renderer.add_point_light(&red_point_light);
        renderer.add_point_light(&yellow_point_light);
        renderer.add_point_light(&green_point_light);
        renderer.add_point_light(&blue_point_light);

        if (torch)
        {
            renderer.add_spot_light(&torch_spot_light);
        }

        renderer.set_skybox(&skybox_cubemap);

        renderer.add_water(&test_water);

        // renderer.add_sprite(&grass_sprite);

        // render everything
        renderer.draw(&main_camera, (float)window_width / (float)window_height, SDL_GetTicks(), delta_time);

        // display the window
        SDL_GL_SwapWindow(window);

        // apply fps cap
        unsigned int frame_end = SDL_GetTicks();
        unsigned int frame_time = frame_end - frame_start;

        if (frame_delay > frame_time)
        {
            SDL_Delay(frame_delay - frame_time);
        }
    }

    // close audio
    Mix_CloseAudio();

    // close window
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);

    // close SDL
    SDLNet_Quit();
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
