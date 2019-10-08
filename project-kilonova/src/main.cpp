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
#include "terrain.hpp"
#include "texture.hpp"
#include "vertex.hpp"
#include "water.hpp"

constexpr const char *window_title = "Project Kilonova";
constexpr int window_width = 1280;
constexpr int window_height = 720;
constexpr const char *version = "0.0.1";

constexpr int fps_cap = 300;
constexpr int frame_delay = 1000 / fps_cap;

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            std::cout << "Options:" << std::endl;
            std::cout << "  -h, --help\tPrint this message" << std::endl;
            std::cout << "  -v, --version\tPrint version information" << std::endl;
        }
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            std::cout << version << std::endl;
        }
    }

    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_GL_SetSwapInterval(0);
    SDL_Window *window = SDL_CreateWindow(
        window_title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    Mix_Init(0);
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);
    TTF_Init();
    SDLNet_Init();

    pk::renderer renderer(window_width, window_height);
    pk::audio audio;

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

    pk::texture iron_albedo_texture("assets/images/iron_albedo.png");
    pk::texture iron_normal_texture("assets/images/iron_normal.png");
    pk::texture iron_metallic_texture("assets/images/iron_metallic.png");
    pk::texture iron_roughness_texture("assets/images/iron_roughness.png");
    pk::texture iron_ao_texture("assets/images/iron_ao.png");

    pk::cubemap skybox_cubemap(
        {"assets/images/sky/right.jpg",
         "assets/images/sky/left.jpg",
         "assets/images/sky/top.jpg",
         "assets/images/sky/bottom.jpg",
         "assets/images/sky/front.jpg",
         "assets/images/sky/back.jpg"});

    pk::sound bounce_sound("assets/audio/bounce.wav");
    pk::sound shoot_sound("assets/audio/shoot.wav");

    pk::material default_material(
        &iron_albedo_texture,
        glm::vec3(1.0f, 1.0f, 1.0f),
        &iron_normal_texture,
        &iron_metallic_texture,
        &iron_roughness_texture,
        &iron_ao_texture);

    pk::object floor_object(
        &cube_mesh,
        &default_material,
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
        &default_material,
        glm::vec3(2.0f, -0.5f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f));
    pk::object box_3_object(
        &cube_mesh,
        &default_material,
        glm::vec3(0.0f, -0.5f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f));
    pk::object box_4_object(
        &cube_mesh,
        &default_material,
        glm::vec3(-2.0f, -0.5f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f));
    pk::object box_5_object(
        &cube_mesh,
        &default_material,
        glm::vec3(0.0f, -0.5f, -2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f));

    pk::directional_light main_directional_light(
        glm::vec3(-0.2f, -1.0f, -0.3f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    pk::point_light red_point_light(
        glm::vec3(2.0f, 0.0f, 2.0f),
        glm::vec3(1.0f, 0.0f, 0.0f));
    pk::point_light yellow_point_light(
        glm::vec3(-2.0f, 0.0f, -2.0f),
        glm::vec3(1.0f, 1.0f, 0.0f));
    pk::point_light green_point_light(
        glm::vec3(2.0f, 0.0f, -2.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    pk::point_light blue_point_light(
        glm::vec3(-2.0f, 0.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 1.0f));

    pk::spot_light torch_spot_light(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        cosf(glm::radians(12.5f)),
        cosf(glm::radians(15.0f)));

    pk::water test_water(
        glm::vec3(0.0f, -2.0f, 0.0f),
        glm::vec2(100.0f, 100.0f));

    pk::terrain test_terrain(0, 0, &default_material);

    pk::sprite grass_sprite(
        &iron_albedo_texture,
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),
        0.0f,
        glm::vec2(1.0f, 1.0f));

    pk::camera main_camera(
        glm::vec3(0.0f, 0.0f, 3.0f),
        0.0f,
        -90.0f,
        0.0f,
        45.0f);

    pk::source origin_source;
    pk::source camera_source;

    unsigned int current_time = 0;
    float fps_update_timer = 0.0f;
    bool torch = true;
    float bounce_timer = 0.0f;
    float shoot_timer = 0.0f;

    bool quit = false;
    while (!quit)
    {
        unsigned int frame_start = SDL_GetTicks();
        unsigned int previous_time = current_time;
        current_time = frame_start;
        float delta_time = (current_time - previous_time) / 1000.0f;

        fps_update_timer += delta_time;
        if (fps_update_timer > 0.25f)
        {
            fps_update_timer = 0.0f;
            char title[256];
            sprintf(title, "%s %s - FPS: %d", window_title, version, (int)(1 / delta_time));
            SDL_SetWindowTitle(window, title);
        }

        int num_keys;
        const unsigned char *keys = SDL_GetKeyboardState(&num_keys);
        int mouse_x, mouse_y;
        unsigned int mouse = SDL_GetMouseState(&mouse_x, &mouse_y);
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
                    renderer.set_render_size(width, height);
                    std::cout << "Window resized to " << width << "x" << height << std::endl;
                }
                break;
                }
            }
            break;
            }
        }

        float speed = 5.0f * delta_time;
        if (keys[SDL_SCANCODE_LSHIFT])
        {
            speed *= 2.0f;
        }
        if ((keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_D]) ||
            (keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_A]) ||
            (keys[SDL_SCANCODE_S] && keys[SDL_SCANCODE_D]) ||
            (keys[SDL_SCANCODE_S] && keys[SDL_SCANCODE_A]))
        {
            // precomputed 1 / sqrt(2)
            speed *= 0.71f;
        }
        glm::vec3 main_camera_front = main_camera.calc_front();
        glm::vec3 main_camera_up = main_camera.calc_up();
        if (keys[SDL_SCANCODE_W])
        {
            main_camera.position += main_camera_front * speed;
        }
        if (keys[SDL_SCANCODE_A])
        {
            main_camera.position -= glm::normalize(glm::cross(main_camera_front, main_camera_up)) * speed;
        }
        if (keys[SDL_SCANCODE_S])
        {
            main_camera.position -= main_camera_front * speed;
        }
        if (keys[SDL_SCANCODE_D])
        {
            main_camera.position += glm::normalize(glm::cross(main_camera_front, main_camera_up)) * speed;
        }

        float angle = current_time * 0.001f;
        float angle_sin = sinf(angle);
        float angle_cos = cosf(angle);
        box_1_object.rotation.x = angle_sin;
        box_1_object.rotation.y = angle_cos;
        main_directional_light.direction.x = angle_sin;
        main_directional_light.direction.z = angle_cos;
        torch_spot_light.position = main_camera.position;
        torch_spot_light.direction = main_camera_front;

        audio.set_listener(main_camera.position, main_camera_front, main_camera_up);
        camera_source.set_position(main_camera.position);

        shoot_timer += delta_time;
        if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            if (shoot_timer >= 0.25f)
            {
                shoot_timer = 0.0f;
                camera_source.play(&shoot_sound);
            }
        }

        bounce_timer += delta_time;
        if (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT))
        {
            if (bounce_timer >= 0.25f)
            {
                bounce_timer = 0.0f;
                origin_source.play(&bounce_sound);
            }
        }

        renderer.add_object(&floor_object);
        renderer.add_object(&box_1_object);
        renderer.add_object(&box_2_object);
        renderer.add_object(&box_3_object);
        renderer.add_object(&box_4_object);
        renderer.add_object(&box_5_object);
        renderer.add_directional_light(&main_directional_light);
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
        renderer.add_terrain(&test_terrain);
        // renderer.add_sprite(&grass_sprite);
        int width, height;
        SDL_GetWindowSize(window, &width, &height);
        renderer.flush(&main_camera, (float)width / (float)height, SDL_GetTicks(), delta_time);

        SDL_GL_SwapWindow(window);

        unsigned int frame_end = SDL_GetTicks();
        unsigned int frame_time = frame_end - frame_start;
        if (frame_delay > frame_time)
        {
            SDL_Delay(frame_delay - frame_time);
        }
    }

    SDLNet_Quit();
    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
