#include <cxxopts.hpp>
#include <iostream>
#include <imgui.h>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "audio.hpp"
#include "display.hpp"
#include "empty_scene.hpp"
#include "game_scene.hpp"
#include "pause_scene.hpp"
#include "renderer.hpp"
#include "scene.hpp"

#define WINDOW_TITLE "Project Kilonova"
#define VERSION "v0.0.1"

int main(int argc, char *argv[])
{
    int window_width;
    int window_height;
    float render_scale;

    try
    {
        cxxopts::Options options("pk");

        auto option_adder = options.add_options();
        option_adder("height", "Set window height", cxxopts::value<int>()->default_value("720"));
        option_adder("h,help", "Print usage");
        option_adder("scale", "Set render scale", cxxopts::value<float>()->default_value("1.0"));
        option_adder("v,version", "Print version");
        option_adder("width", "Set window width", cxxopts::value<int>()->default_value("1280"));

        auto result = options.parse(argc, argv);
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
        return 0;
    }

    pk::display display(WINDOW_TITLE, window_width, window_height);
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
                    spdlog::info("Window resized to {}x{}", window_width, window_height);
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
                        spdlog::info("Render scale changed to {}", render_scale);
                    }
                    break;
                    case SDLK_EQUALS:
                    {
                        if (render_scale < 1.0f)
                        {
                            render_scale += 0.1f;
                        }
                        renderer.set_screen_size(window_width, window_height, render_scale);
                        spdlog::info("Render scale changed to {}", render_scale);
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
