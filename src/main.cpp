#include <algorithm>
#include <iostream>

#include "atlas.hpp"
#include "audio.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "directional_light.hpp"
#include "display.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "point_light.hpp"
#include "program.hpp"
#include "renderer.hpp"
#include "skybox.hpp"
#include "sound.hpp"
#include "source.hpp"
#include "spot_light.hpp"
#include "sprite.hpp"
#include "terrain.hpp"
#include "texture.hpp"
#include "vertex.hpp"
#include "water.hpp"

constexpr float pi = 3.14159f;

// TODO: assimp

// TODO: physics

// TODO: scene graph

int main(int argc, char *argv[])
{
    config_load();

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
        if (strcmp(argv[i], "--width") == 0)
        {
            window_width = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "--height") == 0)
        {
            window_height = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "--scale") == 0)
        {
            render_scale = std::clamp((float)atof(argv[i + 1]), 0.1f, 1.0f);
        }
    }

    pk::display display(window_title, window_width, window_height);
    pk::renderer renderer(
        window_width, window_height, render_scale,
        window_width, window_height,
        window_width, window_height);
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

    pk::texture aluminum_albedo_texture("assets/images/aluminum_albedo.png", true);
    pk::texture aluminum_normal_texture("assets/images/aluminum_normal.png");
    pk::texture aluminum_metallic_texture("assets/images/aluminum_metallic.png");
    pk::texture aluminum_roughness_texture("assets/images/aluminum_roughness.png");
    pk::texture aluminum_ao_texture("assets/images/aluminum_ao.png");
    pk::texture cloth_albedo_texture("assets/images/cloth_albedo.png", true);
    pk::texture cloth_normal_texture("assets/images/cloth_normal.png");
    pk::texture cloth_metallic_texture("assets/images/cloth_metallic.png");
    pk::texture cloth_roughness_texture("assets/images/cloth_roughness.png");
    pk::texture cloth_ao_texture("assets/images/cloth_ao.png");
    pk::texture cloth_height_texture("assets/images/cloth_height.png");
    pk::texture grass_albedo_texture("assets/images/grass_albedo.png", true);
    pk::texture grass_normal_texture("assets/images/grass_normal.png");
    pk::texture grass_metallic_texture("assets/images/grass_metallic.png");
    pk::texture grass_roughness_texture("assets/images/grass_roughness.png");
    pk::texture grass_ao_texture("assets/images/grass_ao.png");
    pk::texture grass_height_texture("assets/images/grass_height.png");
    pk::texture ground_albedo_texture("assets/images/ground_albedo.png", true);
    pk::texture ground_normal_texture("assets/images/ground_normal.png");
    pk::texture ground_metallic_texture("assets/images/ground_metallic.png");
    pk::texture ground_roughness_texture("assets/images/ground_roughness.png");
    pk::texture ground_ao_texture("assets/images/ground_ao.png");
    pk::texture ground_height_texture("assets/images/ground_height.png");
    pk::texture iron_albedo_texture("assets/images/iron_albedo.png", true);
    pk::texture iron_normal_texture("assets/images/iron_normal.png");
    pk::texture iron_metallic_texture("assets/images/iron_metallic.png");
    pk::texture iron_roughness_texture("assets/images/iron_roughness.png");
    pk::texture iron_ao_texture("assets/images/iron_ao.png");
    pk::texture rock_albedo_texture("assets/images/rock_albedo.png", true);
    pk::texture rock_normal_texture("assets/images/rock_normal.png");
    pk::texture rock_metallic_texture("assets/images/rock_metallic.png");
    pk::texture rock_roughness_texture("assets/images/rock_roughness.png");
    pk::texture rock_ao_texture("assets/images/rock_ao.png");
    pk::texture rock_height_texture("assets/images/rock_height.png");
    pk::texture wood_albedo_texture("assets/images/wood_albedo.png", true);
    pk::texture wood_normal_texture("assets/images/wood_normal.png");
    pk::texture wood_metallic_texture("assets/images/wood_metallic.png");
    pk::texture wood_roughness_texture("assets/images/wood_roughness.png");
    pk::texture wood_ao_texture("assets/images/wood_ao.png");
    pk::texture grass_texture("assets/images/grass_sprite.png", true);

    pk::material aluminum_material(
        &aluminum_albedo_texture,
        glm::vec3(1.0f, 1.0f, 1.0f),
        &aluminum_normal_texture,
        &aluminum_metallic_texture,
        &aluminum_roughness_texture,
        &aluminum_ao_texture,
        nullptr);
    pk::material cloth_material(
        &cloth_albedo_texture,
        glm::vec3(1.0f, 1.0f, 1.0f),
        &cloth_normal_texture,
        &cloth_metallic_texture,
        &cloth_roughness_texture,
        &cloth_ao_texture,
        &cloth_height_texture);
    pk::material grass_material(
        &grass_albedo_texture,
        glm::vec3(1.0f, 1.0f, 1.0f),
        &grass_normal_texture,
        &grass_metallic_texture,
        &grass_roughness_texture,
        &grass_ao_texture,
        &grass_height_texture);
    pk::material ground_material(
        &ground_albedo_texture,
        glm::vec3(1.0f, 1.0f, 1.0f),
        &ground_normal_texture,
        &ground_metallic_texture,
        &ground_roughness_texture,
        &ground_ao_texture,
        &ground_height_texture);
    pk::material iron_material(
        &iron_albedo_texture,
        glm::vec3(1.0f, 1.0f, 1.0f),
        &iron_normal_texture,
        &iron_metallic_texture,
        &iron_roughness_texture,
        &iron_ao_texture,
        nullptr);
    pk::material rock_material(
        &rock_albedo_texture,
        glm::vec3(1.0f, 1.0f, 1.0f),
        &rock_normal_texture,
        &rock_metallic_texture,
        &rock_roughness_texture,
        &rock_ao_texture,
        &rock_height_texture);
    pk::material wood_material(
        &wood_albedo_texture,
        glm::vec3(1.0f, 1.0f, 1.0f),
        &wood_normal_texture,
        &wood_metallic_texture,
        &wood_roughness_texture,
        &wood_ao_texture,
        nullptr);

    pk::object floor_object(
        &cube_mesh,
        &ground_material,
        glm::vec3(0.0f, -2.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(10.0f, 1.0f, 10.0f));
    pk::object box_1_object(
        &cube_mesh,
        &iron_material,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));
    pk::object box_2_object(
        &cube_mesh,
        &cloth_material,
        glm::vec3(4.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));
    pk::object box_3_object(
        &cube_mesh,
        &rock_material,
        glm::vec3(0.0f, 0.0f, 4.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));
    pk::object box_4_object(
        &cube_mesh,
        &wood_material,
        glm::vec3(-4.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));
    pk::object box_5_object(
        &cube_mesh,
        &aluminum_material,
        glm::vec3(0.0f, 0.0f, -4.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    const float sun_intensity = 10.0f;
    pk::directional_light sun_directional_light(
        glm::vec3(0.352286f, -0.547564f, -0.758992f),
        glm::vec3(1.0f, 1.0f, 1.0f) * sun_intensity,
        4096);

    const float point_light_intensity = 10.0f;
    pk::point_light red_point_light(
        glm::vec3(2.0f, 0.0f, 2.0f),
        glm::vec3(1.0f, 0.0f, 0.0f) * point_light_intensity,
        512);
    pk::point_light yellow_point_light(
        glm::vec3(-2.0f, 0.0f, -2.0f),
        glm::vec3(1.0f, 1.0f, 0.0f) * point_light_intensity,
        512);
    pk::point_light green_point_light(
        glm::vec3(2.0f, 0.0f, -2.0f),
        glm::vec3(0.0f, 1.0f, 0.0f) * point_light_intensity,
        512);
    pk::point_light blue_point_light(
        glm::vec3(-2.0f, 0.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 1.0f) * point_light_intensity,
        512);

    const float torch_intensity = 20.0f;
    pk::spot_light torch_spot_light(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f) * torch_intensity,
        cosf(glm::radians(12.5f)),
        cosf(glm::radians(15.0f)),
        1024);

    pk::water test_water(
        glm::vec3(0.0f, -2.0f, 0.0f),
        glm::vec2(100.0f, 100.0f));

    pk::terrain test_terrain(0, 0, &grass_material);

    pk::skybox skybox("assets/images/GCanyon_C_YumaPoint_8k.jpg");

    pk::sprite grass_sprite(
        &grass_texture,
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),
        0.0f,
        glm::vec2(1.0f, 1.0f));

    pk::sound ambient_sound("assets/audio/ambient.wav");
    pk::sound bounce_sound("assets/audio/bounce.wav");
    pk::sound shoot_sound("assets/audio/shoot.wav");

    pk::source ambient_source(glm::vec3(0.0f, 0.0f, 0.0f));
    pk::source bounce_source(glm::vec3(0.0f, 0.0f, 0.0f));
    pk::source shoot_source(glm::vec3(0.0f, 0.0f, 0.0f));

    pk::camera main_camera(
        glm::vec3(0.0f, 0.0f, 3.0f),
        0.0f,
        -90.0f,
        0.0f,
        45.0f);

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_GL_SetSwapInterval(0);

    ambient_source.set_loop(true);
    ambient_source.set_gain(0.5f);
    ambient_source.play(&ambient_sound);

    unsigned int current_time = 0;
    float fps_update_timer = 0.0f;
    float time_scale = 1.0f;
    bool torch_on = true;
    bool torch_follow = true;

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
            sprintf(title, "%s - FPS: %d", window_title, (int)(1 / delta_time));
            display.set_title(title);
        }

        delta_time *= time_scale;

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
                    torch_on = !torch_on;
                }
                break;
                case SDLK_g:
                {
                    torch_follow = !torch_follow;
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
                    // TODO: take into account roll
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
                main_camera.fov -= event.wheel.y;
                if (main_camera.fov <= 1.0f)
                {
                    main_camera.fov = 1.0f;
                }
                if (main_camera.fov >= 120.0f)
                {
                    main_camera.fov = 120.0f;
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
                    window_width = event.window.data1;
                    window_height = event.window.data2;
                    display.set_window_size(window_width, window_height);
                    renderer.set_screen_size(window_width, window_height, render_scale);
                    renderer.set_reflection_size(window_width, window_height);
                    renderer.set_refraction_size(window_width, window_height);
                    std::cout << "Window resized to " << window_width << "x" << window_height << std::endl;
                }
                break;
                }
            }
            break;
            }
        }

        if (keys[SDL_SCANCODE_Q])
        {
            main_camera.roll--;
        }
        if (keys[SDL_SCANCODE_E])
        {
            main_camera.roll++;
        }

        glm::vec3 main_camera_front = main_camera.calc_front();
        glm::vec3 main_camera_right = main_camera.calc_right();
        glm::vec3 main_camera_up = main_camera.calc_up();

        static glm::vec3 velocity(0.0f, 0.0f, 0.0f);
        glm::vec3 acceleration(0.0f, 0.0f, 0.0f);
        if (keys[SDL_SCANCODE_W])
        {
            acceleration += main_camera_front;
        }
        if (keys[SDL_SCANCODE_A])
        {
            acceleration -= main_camera_right;
        }
        if (keys[SDL_SCANCODE_S])
        {
            acceleration -= main_camera_front;
        }
        if (keys[SDL_SCANCODE_D])
        {
            acceleration += main_camera_right;
        }
        if (keys[SDL_SCANCODE_SPACE])
        {
            acceleration += main_camera_up;
        }
        if (keys[SDL_SCANCODE_LCTRL])
        {
            acceleration -= main_camera_up;
        }
        float acceleration_length = glm::length(acceleration);
        if (acceleration_length > 1.0f)
        {
            acceleration /= acceleration_length;
        }
        float speed = 50.0f;
        if (keys[SDL_SCANCODE_LSHIFT])
        {
            speed *= 2.0f;
        }
        acceleration *= speed;
        acceleration -= velocity * 10.0f;
        main_camera.position = 0.5f * acceleration * powf(delta_time, 2.0f) + velocity * delta_time + main_camera.position;
        velocity = acceleration * delta_time + velocity;

        static float angle = 0.0f;
        angle += 0.5f * delta_time;
        if (angle > 2 * pi)
        {
            angle = 0;
        }
        float distance = 6.0f;
        red_point_light.position.x = distance * sinf(angle);
        red_point_light.position.z = distance * cosf(angle);
        yellow_point_light.position.x = distance * sinf(angle + (pi / 2));
        yellow_point_light.position.z = distance * cosf(angle + (pi / 2));
        green_point_light.position.x = distance * sinf(angle + pi);
        green_point_light.position.z = distance * cosf(angle + pi);
        blue_point_light.position.x = distance * sinf(angle + 3 * pi / 2);
        blue_point_light.position.z = distance * cosf(angle + 3 * pi / 2);

        if (torch_follow)
        {
            torch_spot_light.position = main_camera.position;
            torch_spot_light.direction = glm::mix(torch_spot_light.direction, main_camera_front, 30.0f * delta_time);
        }

        audio.set_listener(main_camera.position, main_camera_front, main_camera_up);
        ambient_source.set_position(main_camera.position);
        shoot_source.set_position(main_camera.position);

        if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            if (!shoot_source.is_playing())
            {
                shoot_source.play(&shoot_sound);
            }
        }

        if (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT))
        {
            if (!bounce_source.is_playing())
            {
                bounce_source.play(&bounce_sound);
            }
        }

        display.make_current();

        renderer.add_object(&floor_object);
        renderer.add_object(&box_1_object);
        renderer.add_object(&box_2_object);
        renderer.add_object(&box_3_object);
        renderer.add_object(&box_4_object);
        renderer.add_object(&box_5_object);
        renderer.add_directional_light(&sun_directional_light);
        renderer.add_point_light(&red_point_light);
        renderer.add_point_light(&yellow_point_light);
        renderer.add_point_light(&green_point_light);
        renderer.add_point_light(&blue_point_light);
        if (torch_on)
        {
            renderer.add_spot_light(&torch_spot_light);
        }
        renderer.add_water(&test_water);
        renderer.add_terrain(&test_terrain);
        // renderer.add_sprite(&grass_sprite);
        renderer.flush(&main_camera, &skybox, current_time, delta_time);

        display.swap();

        unsigned int frame_end = SDL_GetTicks();
        unsigned int frame_time = frame_end - frame_start;
        if (frame_delay > frame_time)
        {
            SDL_Delay(frame_delay - frame_time);
        }
    }

    config_save();

    return 0;
}
