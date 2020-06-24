#include <algorithm>
#include <iostream>

#include "atlas.hpp"
#include "audio.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "directional_light.hpp"
#include "display.hpp"
#include "mesh.hpp"
#include "model.hpp"
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
            std::cout << "  -h, --help       Print this message" << std::endl;
            std::cout << "  --width <int>    Set window width" << std::endl;
            std::cout << "  --height <int>   Set window height" << std::endl;
            std::cout << "  --scale <float>  Set render scale" << std::endl;
            std::cout << "  -v, --version    Print version information" << std::endl;
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

    pk::model test_model("assets/models/backpack/backpack.obj");
    pk::object test_object(
        &test_model,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f));

    pk::terrain test_terrain(0, 0);

    pk::skybox skybox("assets/images/GCanyon_C_YumaPoint_8k.jpg");

    // pk::sprite grass_sprite(
    //     &grass_texture,
    //     glm::vec3(1.0f, 1.0f, 1.0f),
    //     glm::vec2(0.0f, 0.0f),
    //     0.0f,
    //     glm::vec2(1.0f, 1.0f));

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

        renderer.add_object(&test_object);
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
        // renderer.add_terrain(&test_terrain);
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
