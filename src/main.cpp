#include <iostream>
#include <imgui.h>
#include <SDL2/SDL.h>

#include "audio.hpp"
#include "display.hpp"
#include "empty_scene.hpp"
#include "game_scene.hpp"
#include "pause_scene.hpp"
#include "renderer.hpp"
#include "scene.hpp"

constexpr const char *window_title = "Project Kilonova";
constexpr const char *version = "0.0.1";

int window_width = 1280;
int window_height = 720;
float render_scale = 1.0f;

int main(int argc, char *argv[])
{
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
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            std::cout << window_title << " " << version << std::endl;
        }
        else if (strcmp(argv[i], "--width") == 0)
        {
            window_width = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "--height") == 0)
        {
            window_height = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "--scale") == 0)
        {
            render_scale = glm::clamp((float)atof(argv[i + 1]), 0.1f, 1.0f);
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

    unsigned int current_time = 0;
    float time_scale = 1.0f;
    bool console_open = false;
    bool wireframe = false;

    bool quit = false;
    while (!quit)
    {
        unsigned int previous_time = current_time;
        current_time = SDL_GetTicks();
        float delta_time = ((current_time - previous_time) / 1000.0f) * time_scale;

        int num_keys;
        const unsigned char *keys = SDL_GetKeyboardState(&num_keys);
        SDL_Event event;
        while (display.poll_event(&event))
        {
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
            case SDL_KEYDOWN:
            {
                if (!io.WantCaptureKeyboard)
                {
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_RETURN:
                    {
                        if (keys[SDL_SCANCODE_LALT])
                        {
                            display.toggle_fullscreen();
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
            }

            scene = scene->handle_event(event);
            if (!scene)
            {
                quit = true;
            }
        }

        scene = scene->update(&audio, delta_time);
        if (!scene)
        {
            quit = true;
        }

        display.make_current();

        if (scene)
        {
            scene->render(&renderer);
        }

        renderer.wireframe = wireframe;
        renderer.flush(current_time, delta_time);

        display.start_gui();

        if (scene)
        {
            scene->gui();
        }

        if (console_open)
        {
            ImGui::Begin("Console", &console_open);

            static std::vector<std::string> messages;

            // TODO: command arguments
            char command[256];
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
                else if (strcmp(command, "map") == 0)
                {
                    if (scene)
                    {
                        delete scene;
                    }
                    scene = new pk::game_scene();
                }
                else if (strcmp(command, "twf") == 0)
                {
                    wireframe = !wireframe;
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

        display.end_gui();

        display.swap();
    }

    if (scene)
    {
        delete scene;
    }

    return 0;
}
