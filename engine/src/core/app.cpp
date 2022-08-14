#include <liminal/core/app.hpp>

#include <AL/al.h>
#include <SDL2/SDL.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <cxxopts.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <iostream>
#include <liminal/core/assets.hpp>
#include <liminal/core/scene.hpp>
#include <liminal/input/input.hpp>
#include <spdlog/spdlog.h>
#include <string>

constexpr const char *version_string = "v0.0.1";
constexpr const char *window_title = "Liminal Engine";

liminal::app::app(int argc, char *argv[])
{
    // parse command line options
    try
    {
        cxxopts::Options options(argv[0]);

        auto option_adder = options.add_options();
        option_adder("width", "Set window width", cxxopts::value<int>()->default_value("1280"));
        option_adder("height", "Set window height", cxxopts::value<int>()->default_value("720"));
        option_adder("scale", "Set render scale", cxxopts::value<float>()->default_value("1.0"));
        option_adder("v,version", "Print version");
        option_adder("h,help", "Print usage");

        const auto result = options.parse(argc, argv);

        window_height = result["height"].as<int>();
        window_width = result["width"].as<int>();
        render_scale = glm::clamp(result["scale"].as<float>(), 0.1f, 1.0f);

        if (result.count("help"))
        {
            std::cout << options.help() << std::endl;
            return;
        }

        if (result.count("version"))
        {
            std::cout << version_string << std::endl;
            return;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return;
    }

    // init subsystems
    sdl = std::make_unique<liminal::sdl>();
    sdl_image = std::make_unique<liminal::sdl_image>();
    sdl_mixer = std::make_unique<liminal::sdl_mixer>();
    audio = std::make_unique<liminal::audio>();
    window = std::make_unique<liminal::window>(window_title, window_width, window_height);
    gl_context = std::make_unique<liminal::gl_context>(window->get_sdl_window());
    glew = std::make_unique<liminal::glew>();
    imgui_context = std::make_unique<liminal::imgui_context>(
        window->get_sdl_window(),
        gl_context->get_sdl_gl_context());
    al_device = std::make_unique<liminal::al_device>();
    al_context = std::make_unique<liminal::al_context>(al_device->get_alc_device());
    renderer = std::make_unique<liminal::renderer>(
        window_width, window_height, render_scale,
        4096, 512, 1024,
        window_width, window_height,
        window_width, window_height);
}

void liminal::app::run()
{
    std::uint64_t current_time = 0;
    while (running)
    {
        // calculate time between frames
        const auto previous_time = current_time;
        current_time = sdl->get_ticks();
        const auto delta_time = (current_time - previous_time) / 1000.0f;

        // gather input
        liminal::input::last_keys = liminal::input::keys;
        const auto keys = sdl->get_keys();
        for (std::size_t scancode = 0; scancode < liminal::input::keys.size(); scancode++)
        {
            liminal::input::keys.at(scancode) = keys[scancode];
        }

        liminal::input::last_mouse_buttons = liminal::input::mouse_buttons;
        const auto mouse_buttons = sdl->get_mouse_state(&liminal::input::mouse_x, &liminal::input::mouse_y);
        for (std::size_t button = 0; button < liminal::input::mouse_buttons.size(); button++)
        {
            liminal::input::mouse_buttons.at(button) = mouse_buttons & SDL_BUTTON(button);
        }

        liminal::input::mouse_dx = 0;
        liminal::input::mouse_dy = 0;
        liminal::input::mouse_wheel_x = 0;
        liminal::input::mouse_wheel_y = 0;

        // process events
        SDL_Event event;
        while (sdl->poll_event(event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
            {
                stop();
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
                    window->set_size(window_width, window_height);
                    resize(window_width, window_height);
                    spdlog::info("Window resized to {}x{}", window_width, window_height);
                }
                break;
                }
            }
            break;
            case SDL_MOUSEMOTION:
            {
                liminal::input::mouse_dx = event.motion.xrel;
                liminal::input::mouse_dy = event.motion.yrel;
            }
            break;
            case SDL_MOUSEWHEEL:
            {
                liminal::input::mouse_wheel_x = event.wheel.x;
                liminal::input::mouse_wheel_y = event.wheel.y;
            }
            break;
            }

            imgui_context->handle_event(event);
        }

        if (!ImGui::GetIO().WantCaptureKeyboard)
        {
            if (liminal::input::key_down(liminal::keycode::RETURN) &&
                liminal::input::key(liminal::keycode::LALT))
            {
                window->toggle_fullscreen();
            }

            if (liminal::input::key_down(liminal::keycode::GRAVE))
            {
                console_open = !console_open;
            }

            if (liminal::input::key_down(liminal::keycode::F4) &&
                liminal::input::key(liminal::keycode::LALT))
            {
                stop();
            }
        }

        // start of frame
        imgui_context->begin_frame();

        // update app
        update(current_time, delta_time * time_scale);

        // render console
        if (console_open)
        {
            if (ImGui::Begin("Console", &console_open))
            {
                // TODO: no local static!!!
                // should add proper engine logging and have the console display those logs
                static std::vector<std::string> messages;

                char command_c_str[256] = {};
                if (ImGui::InputText("Input", command_c_str, sizeof(command_c_str), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    std::string command(command_c_str);

                    if (command == "help")
                    {
                        messages.push_back("TODO: help");
                    }
                    else if (command == "quit")
                    {
                        stop();
                    }
                    else if (command == "wireframe")
                    {
                        renderer->wireframe = !renderer->wireframe;
                        if (renderer->wireframe)
                        {
                            messages.push_back("Wireframe on");
                        }
                        else
                        {
                            messages.push_back("Wireframe off");
                        }
                    }
                    else if (command == "greyscale")
                    {
                        renderer->greyscale = !renderer->greyscale;
                        if (renderer->greyscale)
                        {
                            messages.push_back("Greyscale on");
                        }
                        else
                        {
                            messages.push_back("Greyscale off");
                        }
                    }
                    else if (command == "reload")
                    {
                        renderer->reload_programs();
                        // TODO: reload lua scripts?
                    }
                    else if (command == "render_scale 1") // TODO: console commands with arguments
                    {
                        render_scale = 1;

                        renderer->set_render_scale(render_scale);
                        spdlog::info("Render scale changed to {}", render_scale);
                    }
                    else if (command == "render_scale 0.5")
                    {
                        render_scale = 0.5f;

                        renderer->set_render_scale(render_scale);
                        spdlog::info("Render scale changed to {}", render_scale);
                    }
                    else if (command == "render_scale 0.1")
                    {
                        render_scale = 0.1f;

                        renderer->set_render_scale(render_scale);
                        spdlog::info("Render scale changed to {}", render_scale);
                    }
                    else if (command == "toggle_slomo")
                    {
                        if (time_scale > 0.25f)
                        {
                            time_scale = 0.25f;
                        }
                        else
                        {
                            time_scale = 1;
                        }
                    }
                    else
                    {
                        messages.push_back("Unknown command");
                    }
                }

                if (ImGui::BeginChild(""))
                {
                    for (const auto &message : messages)
                    {
                        ImGui::Text("%s", message.c_str());
                    }
                }
                ImGui::EndChild();
            }
            ImGui::End();
        }

        // end of frame
        imgui_context->end_frame();
        window->swap();
    }
}

void liminal::app::stop()
{
    running = false;
}

void liminal::app::update(const std::uint64_t, const float)
{
}

void liminal::app::resize(const int width, const int height)
{
    renderer->set_target_size(width, height);
}
