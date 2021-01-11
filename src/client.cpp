#include "client.hpp"

#include <cxxopts.hpp>
#include <iostream>
#include <imgui.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#include <spdlog/spdlog.h>

#include "audio.hpp"
#include "display.hpp"
#include "empty_scene.hpp"
#include "game_scene.hpp"
#include "message.hpp"
#include "pause_scene.hpp"
#include "renderer.hpp"
#include "scene.hpp"

#define SDL_FLAGS (SDL_INIT_AUDIO | SDL_INIT_VIDEO)
#define IMG_FLAGS (IMG_INIT_JPG | IMG_INIT_PNG)
#define MIX_FLAGS (0)

#define WINDOW_TITLE "Project Kilonova"

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 3000

int pk::client_main(cxxopts::ParseResult result)
{
    int window_width = result["width"].as<int>();
    int window_height = result["height"].as<int>();
    int render_scale = glm::clamp(result["scale"].as<float>(), 0.1f, 1.0f);

    if (SDL_Init(SDL_FLAGS) != 0)
    {
        spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
        return 1;
    }

    if (IMG_Init(IMG_FLAGS) != IMG_FLAGS)
    {
        spdlog::error("Failed to initialize SDL_image: {}", IMG_GetError());
        return 1;
    }

    if (Mix_Init(MIX_FLAGS) != MIX_FLAGS)
    {
        spdlog::error("Failed to initialize SDL_mixer: {}", Mix_GetError());
        return 1;
    }

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) != 0)
    {
        spdlog::error("Failed to initialize the mixer API: {}", Mix_GetError());
        return 1;
    }

    if (SDLNet_Init() != 0)
    {
        spdlog::error("Failed to initialize SDL_net: {}", SDLNet_GetError());
        return 1;
    }

    IPaddress server_address;
    if (SDLNet_ResolveHost(&server_address, SERVER_HOST, SERVER_PORT))
    {
        spdlog::error("Failed to resolve host: {}", SDLNet_GetError());
        return 1;
    }

    TCPsocket tcp_socket = SDLNet_TCP_Open(&server_address);
    bool connected = false;
    if (!tcp_socket)
    {
        spdlog::error("Failed to open TCP socket: {}", SDLNet_GetError());
        spdlog::info("Starting in offline mode");
    }
    else
    {
        connected = true;
    }

    UDPsocket udp_socket = SDLNet_UDP_Open(0);
    if (!udp_socket)
    {
        spdlog::error("Failed to open UDP socket: {}", SDLNet_GetError());
        return 1;
    }

    SDLNet_SocketSet socket_set = SDLNet_AllocSocketSet(2);
    if (!socket_set)
    {
        spdlog::error("Failed to allocate socket set: {}", SDLNet_GetError());
        return 1;
    }
    SDLNet_TCP_AddSocket(socket_set, tcp_socket);
    SDLNet_UDP_AddSocket(socket_set, udp_socket);

    int client_id = -1;

    if (connected)
    {
        char buffer[PACKET_SIZE];
        if (SDLNet_TCP_Recv(tcp_socket, buffer, sizeof(buffer)) > 1)
        {
            pk::message *message = (pk::message *)buffer;
            switch (message->type)
            {
            case pk::message_type::MESSAGE_CONNECT_OK:
            {
                pk::id_message *id_message = (pk::id_message *)message;
                client_id = id_message->id;
            }
            break;
            case pk::message_type::MESSAGE_CONNECT_FULL:
            {
                spdlog::error("Server is full");
                return 1;
            }
            break;
            default:
            {
                spdlog::error("Unknown server response");
                return 1;
            }
            break;
            }
        }

        {
            pk::id_message *id_message = new pk::id_message(pk::message_type::MESSAGE_UDP_CONNECT_REQUEST, client_id);
            UDPpacket *packet = SDLNet_AllocPacket(PACKET_SIZE);
            packet->address = server_address;
            packet->data = (unsigned char *)id_message;
            packet->len = sizeof(*id_message);
            if (!SDLNet_UDP_Send(udp_socket, -1, packet))
            {
                spdlog::error("Failed to send UDP packet");
                return 1;
            }
            SDLNet_FreePacket(packet);
        }
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

        if (connected)
        {
            while (SDLNet_CheckSockets(socket_set, 0) > 0)
            {
                if (SDLNet_SocketReady(tcp_socket))
                {
                    char buffer[PACKET_SIZE];
                    if (SDLNet_TCP_Recv(tcp_socket, buffer, sizeof(buffer)) > 1)
                    {
                        pk::message *message = (pk::message *)buffer;
                        switch (message->type)
                        {
                        case pk::message_type::MESSAGE_CONNECT_BROADCAST:
                        {
                            pk::id_message *id_message = (pk::id_message *)message;
                            spdlog::info("Client with ID {} has joined", id_message->id);
                        }
                        break;
                        case pk::message_type::MESSAGE_DISCONNECT_BROADCAST:
                        {
                            pk::id_message *id_message = (pk::id_message *)message;
                            spdlog::info("Client with ID {} has disconnected", id_message->id);
                        }
                        break;
                        case pk::message_type::MESSAGE_CHAT_BROADCAST:
                        {
                            pk::chat_message *chat_message = (pk::chat_message *)message;
                            spdlog::info("Client {}: {}", chat_message->id, chat_message->str);
                        }
                        break;
                        default:
                        {
                            spdlog::error("Unknown TCP packet type: {}", message->type);
                        }
                        break;
                        }
                    }
                }

                if (SDLNet_SocketReady(udp_socket))
                {
                    UDPpacket *packet = SDLNet_AllocPacket(PACKET_SIZE);
                    if (SDLNet_UDP_Recv(udp_socket, packet) == 1)
                    {
                        pk::message *message = (pk::message *)packet->data;
                        switch (message->type)
                        {
                        default:
                        {
                            spdlog::error("Unknown UDP packet type: {}", message->type);
                        }
                        break;
                        }
                    }
                    SDLNet_FreePacket(packet);
                }
            }
        }

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

    if (connected)
    {
        pk::message message = pk::message(pk::message_type::MESSAGE_DISCONNECT_REQUEST);
        SDLNet_TCP_Send(tcp_socket, &message, sizeof(message));
    }

    IMG_Quit();

    Mix_CloseAudio();
    Mix_Quit();

    SDLNet_UDP_DelSocket(socket_set, udp_socket);
    SDLNet_TCP_DelSocket(socket_set, tcp_socket);

    SDLNet_FreeSocketSet(socket_set);

    SDLNet_UDP_Close(udp_socket);
    SDLNet_TCP_Close(tcp_socket);

    SDLNet_Quit();

    SDL_Quit();

    return 0;
}
