#include <iostream>

#include "audio.hpp"
#include "camera.hpp"
#include "cubemap.hpp"
#include "directional_light.hpp"
#include "display.hpp"
#include "hdr_texture.hpp"
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
            std::cout << window_title << " " << version << std::endl;
        }
    }

    int window_width = 1280;
    int window_height = 720;
    float render_scale = 1.0f;
    int depth_map_width = 4096;
    int depth_map_height = 4096;
    int depth_cube_width = 512;
    int depth_cube_height = 512;

    pk::display display(window_title, window_width, window_height);
    pk::renderer renderer(
        window_width, window_height, render_scale,
        window_width / 2, window_height / 2,
        window_width / 2, window_height / 2);
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
        glm::vec3(1.0f, 1.0f, 1.0f),
        depth_map_width, depth_map_height);

    pk::point_light red_point_light(
        glm::vec3(2.0f, 0.0f, 2.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        depth_cube_width, depth_cube_height);
    pk::point_light yellow_point_light(
        glm::vec3(-2.0f, 0.0f, -2.0f),
        glm::vec3(1.0f, 1.0f, 0.0f),
        depth_cube_width, depth_cube_height);
    pk::point_light green_point_light(
        glm::vec3(2.0f, 0.0f, -2.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        depth_cube_width, depth_cube_height);
    pk::point_light blue_point_light(
        glm::vec3(-2.0f, 0.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        depth_cube_width, depth_cube_height);

    pk::spot_light torch_spot_light(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        cosf(glm::radians(12.5f)),
        cosf(glm::radians(15.0f)),
        depth_map_width, depth_map_height);

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

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_GL_SetSwapInterval(0);

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
            display.set_title(title);
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
                case SDLK_MINUS:
                {
                    if (render_scale > 0.2f)
                    {
                        render_scale -= 0.1f;
                    }
                    renderer.set_screen_size(window_width, window_height, render_scale);
                }
                break;
                case SDLK_EQUALS:
                {
                    if (render_scale < 1.0f)
                    {
                        render_scale += 0.1f;
                    }
                    renderer.set_screen_size(window_width, window_height, render_scale);
                }
                break;
                case SDLK_RETURN:
                {
                    if (keys[SDL_SCANCODE_LALT])
                    {
                        display.toggle_fullscreen();
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
                    display.set_window_size(width, height);
                    renderer.set_screen_size(width, height, render_scale);
                    renderer.set_reflection_size(width / 2, height / 2);
                    renderer.set_refraction_size(width / 2, height / 2);
                    std::cout << "Window resized to " << width << "x" << height << std::endl;
                }
                break;
                }
            }
            break;
            }
        }

        glm::vec3 main_camera_front = main_camera.calc_front();
        glm::vec3 main_camera_up = main_camera.calc_up();

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
        torch_spot_light.direction = glm::mix(torch_spot_light.direction, main_camera_front, 30.0f * delta_time);

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
        // renderer.add_terrain(&test_terrain);
        // renderer.add_sprite(&grass_sprite);
        renderer.flush(&main_camera, current_time, delta_time);

        display.swap();

        unsigned int frame_end = SDL_GetTicks();
        unsigned int frame_time = frame_end - frame_start;
        if (frame_delay > frame_time)
        {
            SDL_Delay(frame_delay - frame_time);
        }
    }

    return 0;
}
