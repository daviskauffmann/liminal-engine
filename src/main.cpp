#include <algorithm>
#include <iostream>
#include <imgui.h>

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
#include "scene.hpp"
#include "skybox.hpp"
#include "sound.hpp"
#include "source.hpp"
#include "spot_light.hpp"
#include "sprite.hpp"
#include "terrain.hpp"
#include "texture.hpp"
#include "vertex.hpp"
#include "water.hpp"

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

    pk::scene *scene = new pk::game_scene();

    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = "assets/imgui.ini";

    SDL_GL_SetSwapInterval(0);

    unsigned int current_time = 0;
    float time_scale = 1.0f;

    bool quit = false;
    while (!quit)
    {
        unsigned int previous_time = current_time;
        current_time = SDL_GetTicks();
        float delta_time = ((current_time - previous_time) / 1000.0f) * time_scale;

        int num_keys;
        const unsigned char *keys = SDL_GetKeyboardState(&num_keys);
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_TEXTINPUT:
            {
                io.AddInputCharacter(event.text.text[0]);
            }
            break;
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
                case SDLK_r:
                {
                    if (!io.WantCaptureKeyboard)
                    {
                        renderer.reload_programs();
                    }
                }
                break;
                case SDLK_t:
                {
                    if (!io.WantCaptureKeyboard)
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
                }
                break;
                case SDLK_MINUS:
                {
                    if (!io.WantCaptureKeyboard)
                    {
                        if (render_scale > 0.2f)
                        {
                            render_scale -= 0.1f;
                        }
                        renderer.set_screen_size(window_width, window_height, render_scale);
                        std::cout << "Render scale changed to " << render_scale << std::endl;
                    }
                }
                break;
                case SDLK_EQUALS:
                {
                    if (!io.WantCaptureKeyboard)
                    {
                        if (render_scale < 1.0f)
                        {
                            render_scale += 0.1f;
                        }
                        renderer.set_screen_size(window_width, window_height, render_scale);
                        std::cout << "Render scale changed to " << render_scale << std::endl;
                    }
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
                }
            }
            break;
            case SDL_MOUSEWHEEL:
            {
                if (io.WantCaptureMouse)
                {
                    io.MouseWheel = event.wheel.y;
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

            scene->handle_event(event);
            if (!scene)
            {
                quit = true;
            }
        }

        scene->update(&audio, delta_time);
        if (!scene)
        {
            quit = true;
        }

        display.make_current();

        if (scene)
        {
            scene->render(&renderer);
        }

        renderer.flush(current_time, delta_time);

        if (scene)
        {
            display.start_gui();
            scene->gui();
            display.end_gui();
        }

        display.swap();
    }

    if (scene)
    {
        delete scene;
    }

    config_save();

    return 0;
}
