#include <liminal/core/engine.hpp>

#include <AL/al.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <cxxopts.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <iostream>
#include <liminal/core/app.hpp>
#include <liminal/core/platform.hpp>
#include <liminal/core/scene.hpp>
#include <liminal/graphics/camera.hpp>
#include <liminal/graphics/renderer.hpp>
#include <liminal/input/input.hpp>
#include <SDL2/SDL.h>
#include <string>

constexpr const char *exe_name = "liminal";
constexpr const char *version_string = "v0.0.1";
constexpr const char *window_title = "Liminal Engine";

liminal::engine &liminal::engine::get_instance()
{
    static liminal::engine instance;
    return instance;
}

liminal::engine::engine()
{
}

liminal::engine::~engine()
{
    delete platform;
    delete renderer;
}

void liminal::engine::run(int argc, char *argv[])
{
    // parse command line options
    int window_width;
    int window_height;
    float render_scale;

    try
    {
        cxxopts::Options options(exe_name);

        cxxopts::OptionAdder option_adder = options.add_options();
        option_adder("height", "Set window height", cxxopts::value<int>()->default_value("720"));
        option_adder("h,help", "Print usage");
        option_adder("scale", "Set render scale", cxxopts::value<float>()->default_value("1.0"));
        option_adder("v,version", "Print version");
        option_adder("width", "Set window width", cxxopts::value<int>()->default_value("1280"));

        cxxopts::ParseResult result = options.parse(argc, argv);

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
    platform = new liminal::platform(window_title, window_width, window_height);
    renderer = new liminal::renderer(
        window_width, window_height, render_scale,
        4096, 512, 1024,
        window_width, window_height,
        window_width, window_height);

    // get imgui reference
    ImGuiIO &io = ImGui::GetIO();

    // create client app
    auto app = liminal::create_app(argc, argv);
    auto scene = app->scene;

    // TODO: should camera be an entity?
    auto camera = scene->camera = new liminal::camera(
        glm::vec3(0.0f, 0.0f, 3.0f),
        0.0f,
        0.0f,
        0.0f,
        45.0f);

    unsigned int current_time = 0;
    float time_scale = 1.0f;
    bool console_open = false;

    std::fill(liminal::input::keys.begin(), liminal::input::keys.end(), false);
    std::fill(liminal::input::last_keys.begin(), liminal::input::last_keys.end(), false);

    std::fill(liminal::input::mouse_buttons.begin(), liminal::input::mouse_buttons.end(), false);
    std::fill(liminal::input::last_mouse_buttons.begin(), liminal::input::last_mouse_buttons.end(), false);

    bool quit = false;
    while (!quit)
    {
        // calculate time between frames
        unsigned int previous_time = current_time;
        current_time = SDL_GetTicks();
        float delta_time = ((current_time - previous_time) / 1000.0f) * time_scale;

        // gather input
        liminal::input::last_keys = liminal::input::keys;
        const unsigned char *keys = SDL_GetKeyboardState(nullptr);
        for (int scancode = 0; scancode < liminal::NUM_KEYCODES; scancode++)
        {
            liminal::input::keys[scancode] = keys[scancode];
        }

        liminal::input::last_mouse_buttons = liminal::input::mouse_buttons;
        unsigned int mouse_buttons = SDL_GetMouseState(&liminal::input::mouse_x, &liminal::input::mouse_y);
        for (int button = 0; button < liminal::mouse_button::NUM_MOUSE_BUTTONS; button++)
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
            platform->process_event(&event);

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
                    platform->set_window_size(window_width, window_height);
                    renderer->set_screen_size(window_width, window_height, render_scale);
                    renderer->set_reflection_size(window_width, window_height);
                    renderer->set_refraction_size(window_width, window_height);
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
                    case SDLK_TAB:
                    {
                        SDL_SetRelativeMouseMode((SDL_bool)!SDL_GetRelativeMouseMode());
                    }
                    break;
                    case SDLK_RETURN:
                    {
                        if (liminal::input::key(liminal::KEYCODE_LALT))
                        {
                            platform->toggle_fullscreen();
                        }
                    }
                    break;
                    case SDLK_MINUS:
                    {
                        if (render_scale > 0.2f)
                        {
                            render_scale -= 0.1f;
                        }
                        renderer->set_screen_size(window_width, window_height, render_scale);
                        std::cout << "Render scale changed to " << render_scale << std::endl;
                    }
                    break;
                    case SDLK_EQUALS:
                    {
                        if (render_scale < 1.0f)
                        {
                            render_scale += 0.1f;
                        }
                        renderer->set_screen_size(window_width, window_height, render_scale);
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
                        renderer->reload_programs();
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
                        if (liminal::input::key(liminal::KEYCODE_LALT))
                        {
                            quit = true;
                        }
                    }
                    break;
                    }
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

        // start of frame
        platform->begin_frame();

        // update client app
        app->update(current_time, delta_time);

        // render everything
        // TODO: allow rendering to a texture instead of the screen and exposing/returning the texture to the app
        // the first main use case would be to put the texture in an imgui window in the editor
        renderer->render(*scene, current_time, delta_time);

        // render console
        if (console_open)
        {
            ImGui::Begin("Console", &console_open);

            static std::vector<std::string> messages;

            // TODO: command arguments
            char command[256] = {};
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
                else if (strcmp(command, "wireframe") == 0)
                {
                    renderer->wireframe = !renderer->wireframe;
                    messages.push_back("Wireframe " + renderer->wireframe ? "on" : "off");
                }
                else if (strcmp(command, "greyscale") == 0)
                {
                    renderer->greyscale = !renderer->greyscale;
                    messages.push_back("Greyscale " + renderer->greyscale ? "on" : "off");
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

        // end of frame
        platform->end_frame();
    }

    delete app;
}
