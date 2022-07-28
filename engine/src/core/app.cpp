#include <liminal/core/app.hpp>

#include <AL/al.h>
#include <SDL2/SDL.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <cxxopts.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <iostream>
#include <liminal/core/assets.hpp>
#include <liminal/core/platform.hpp>
#include <liminal/core/scene.hpp>
#include <liminal/graphics/renderer.hpp>
#include <liminal/input/input.hpp>
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
        option_adder("height", "Set window height", cxxopts::value<int>()->default_value("720"));
        option_adder("h,help", "Print usage");
        option_adder("scale", "Set render scale", cxxopts::value<float>()->default_value("1.0"));
        option_adder("v,version", "Print version");
        option_adder("width", "Set window width", cxxopts::value<int>()->default_value("1280"));

        const auto result = options.parse(argc, argv);

        window_height = result["height"].as<int>();

        if (result.count("help"))
        {
            std::cout << options.help() << std::endl;
            return;
        }

        render_scale = glm::clamp(result["scale"].as<float>(), 0.1f, 1.0f);

        if (result.count("version"))
        {
            std::cout << version_string << std::endl;
            return;
        }

        window_width = result["width"].as<int>();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return;
    }

    // init subsystems
    // TODO: dont really like the whole global singleton thing, should find a better solution
    new liminal::assets();
    new liminal::platform(window_title, window_width, window_height);
    new liminal::renderer(
        window_width, window_height, render_scale,
        4096, 512, 1024,
        window_width, window_height,
        window_width, window_height);

    // init input
    std::fill(liminal::input::keys.begin(), liminal::input::keys.end(), false);
    std::fill(liminal::input::last_keys.begin(), liminal::input::last_keys.end(), false);

    std::fill(liminal::input::mouse_buttons.begin(), liminal::input::mouse_buttons.end(), false);
    std::fill(liminal::input::last_mouse_buttons.begin(), liminal::input::last_mouse_buttons.end(), false);
}

liminal::app::~app()
{
    delete liminal::assets::instance;
    delete liminal::platform::instance;
    delete liminal::renderer::instance;
}

void liminal::app::run()
{
    while (running)
    {
        // calculate time between frames
        static unsigned int current_time = 0;
        const auto previous_time = current_time;
        current_time = SDL_GetTicks();
        const auto delta_time = ((current_time - previous_time) / 1000.0f);

        // gather input
        liminal::input::last_keys = liminal::input::keys;
        const auto keys = SDL_GetKeyboardState(nullptr);
        for (std::size_t scancode = 0; scancode < liminal::input::keys.size(); scancode++)
        {
            liminal::input::keys[scancode] = keys[scancode];
        }

        liminal::input::last_mouse_buttons = liminal::input::mouse_buttons;
        const auto mouse_buttons = SDL_GetMouseState(&liminal::input::mouse_x, &liminal::input::mouse_y);
        for (std::size_t button = 0; button < liminal::input::mouse_buttons.size(); button++)
        {
            liminal::input::mouse_buttons[button] = mouse_buttons & SDL_BUTTON(button);
        }

        liminal::input::mouse_dx = 0;
        liminal::input::mouse_dy = 0;
        liminal::input::mouse_wheel_x = 0;
        liminal::input::mouse_wheel_y = 0;

        // process events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            liminal::platform::instance->process_event(&event);

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
                    liminal::platform::instance->set_window_size(window_width, window_height);
                    resize(window_width, window_height);
                    std::cout << "Window resized to " << window_width << "x" << window_height << std::endl;
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
        }

        if (!ImGui::GetIO().WantCaptureKeyboard)
        {
            if (liminal::input::key_down(liminal::keycode::KEYCODE_RETURN) &&
                liminal::input::key(liminal::keycode::KEYCODE_LALT))
            {
                liminal::platform::instance->toggle_fullscreen();
            }

            if (liminal::input::key_down(liminal::keycode::KEYCODE_GRAVE))
            {
                console_open = !console_open;
            }

            if (liminal::input::key_down(liminal::keycode::KEYCODE_F4) &&
                liminal::input::key(liminal::keycode::KEYCODE_LALT))
            {
                stop();
            }
        }

        // start of frame
        liminal::platform::instance->begin_frame();

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
                        liminal::renderer::instance->wireframe = !liminal::renderer::instance->wireframe;
                        messages.push_back("Wireframe " + liminal::renderer::instance->wireframe ? "on" : "off");
                    }
                    else if (command == "greyscale")
                    {
                        liminal::renderer::instance->greyscale = !liminal::renderer::instance->greyscale;
                        messages.push_back("Greyscale " + liminal::renderer::instance->greyscale ? "on" : "off");
                    }
                    else if (command == "reload")
                    {
                        liminal::renderer::instance->reload_programs();
                        // TODO: reload lua scripts?
                    }
                    else if (command == "render_scale 1") // TODO: console commands with arguments
                    {
                        render_scale = 1;

                        liminal::renderer::instance->set_render_scale(render_scale);
                        std::cout << "Render scale changed to " << render_scale << std::endl;
                    }
                    else if (command == "render_scale 0.5")
                    {
                        render_scale = 0.5f;

                        liminal::renderer::instance->set_render_scale(render_scale);
                        std::cout << "Render scale changed to " << render_scale << std::endl;
                    }
                    else if (command == "render_scale 0.1")
                    {
                        render_scale = 0.1f;

                        liminal::renderer::instance->set_render_scale(render_scale);
                        std::cout << "Render scale changed to " << render_scale << std::endl;
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
                    for (auto &message : messages)
                    {
                        ImGui::Text(message.c_str());
                    }
                }
                ImGui::EndChild();
            }
            ImGui::End();
        }

        // end of frame
        liminal::platform::instance->end_frame();
    }
}

void liminal::app::stop()
{
    running = false;
}

void liminal::app::update(const unsigned int, const float)
{
}

void liminal::app::resize(const int width, const int height)
{
    liminal::renderer::instance->set_target_size(width, height);
}
