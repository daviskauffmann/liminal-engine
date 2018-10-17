#include <engine/engine.h>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <stdbool.h>
#include <stdio.h>

#include "../shared/data.h"

#define WINDOW_TITLE "Multiplayer Test"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 1000

#define FPS_CAP 30

int main(int argc, char *argv[])
{
    if (engine_init())
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    if (window_init(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    IPaddress server_address;

    if (net_resolve_host(&server_address, SERVER_HOST, SERVER_PORT))
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    TCPsocket tcp_socket = net_tcp_open(&server_address);

    if (!tcp_socket)
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    TCPpacket *tcp_packet = net_tcp_alloc_packet(PACKET_SIZE);

    if (!tcp_packet)
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    UDPsocket udp_socket = net_udp_open(0);

    if (!udp_socket)
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    UDPpacket *udp_packet = net_udp_alloc_packet(PACKET_SIZE);

    if (!udp_packet)
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    SDLNet_SocketSet socket_set = net_alloc_socket_set(2);

    if (!socket_set)
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    net_tcp_add_socket(socket_set, tcp_socket);
    net_udp_add_socket(socket_set, udp_socket);

    int client_id = -1;

    if (net_tcp_recv(tcp_socket, tcp_packet) == 1)
    {
        struct data *data = (struct data *)tcp_packet->data;

        switch (data->type)
        {
        case DATA_CONNECT_OK:
        {
            struct id_data *id_data = (struct id_data *)data;

            printf("Server assigned ID: %d\n", id_data->id);

            client_id = id_data->id;
        }
        break;
        case DATA_CONNECT_FULL:
        {
            printf("Server is full\n");

            return 1;
        }
        break;
        default:
        {
            printf("TCP: Unknown packet type\n");

            return 1;
        }
        break;
        }
    }

    {
        struct id_data id_data = id_data_create(DATA_UDP_CONNECT_REQUEST, client_id);
        net_udp_send(udp_socket, udp_packet, server_address, &id_data, sizeof(id_data));
    }

    time_cap_fps(FPS_CAP);

    bool quit = false;
    while (!quit)
    {
        time_frame_start();

        int num_keys;
        const unsigned char *keys = window_keyboard(&num_keys);

        int mouse_x, mouse_y;
        unsigned int mouse = window_mouse(&mouse_x, &mouse_y);

        SDL_Event event;
        while (window_event(&event))
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
                case SDLK_RETURN:
                {
                    if (keys[SDL_SCANCODE_LALT])
                    {
                        window_toggle_fullscreen();
                    }
                    else
                    {
                        struct chat_data chat_data = chat_data_create(DATA_CHAT_REQUEST, client_id, "Hello, World!");
                        net_tcp_send(tcp_socket, &chat_data, sizeof(chat_data));
                    }
                }
                break;
                }
            }
            break;
            case SDL_MOUSEBUTTONDOWN:
            {
                struct mouse_data mouse_data = mouse_data_create(DATA_MOUSEDOWN_REQUEST, client_id, event.button.x, event.button.y);
                net_udp_send(udp_socket, udp_packet, server_address, &mouse_data, sizeof(mouse_data));
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

                    window_resize(width, height);
                }
                break;
                }
            }
            break;
            }
        }

        while (SDLNet_CheckSockets(socket_set, 0) > 0)
        {
            if (SDLNet_SocketReady(tcp_socket))
            {
                if (net_tcp_recv(tcp_socket, tcp_packet) == 1)
                {
                    struct data *data = (struct data *)tcp_packet->data;

                    switch (data->type)
                    {
                    case DATA_CONNECT_BROADCAST:
                    {
                        struct id_data *id_data = (struct id_data *)data;

                        printf("Client with ID %d has joined\n", id_data->id);
                    }
                    break;
                    case DATA_DISCONNECT_BROADCAST:
                    {
                        struct id_data *id_data = (struct id_data *)data;

                        printf("Client with ID %d has disconnected\n", id_data->id);
                    }
                    break;
                    case DATA_CHAT_BROADCAST:
                    {
                        struct chat_data *chat_data = (struct chat_data *)data;

                        printf("Client %d: %s\n", chat_data->id, chat_data->message);
                    }
                    break;
                    default:
                    {
                        printf("TCP: Unknown packet type\n");
                    }
                    break;
                    }
                }
            }

            if (SDLNet_SocketReady(udp_socket))
            {
                if (net_udp_recv(udp_socket, udp_packet) == 1)
                {
                    struct data *data = (struct data *)udp_packet->data;

                    switch (data->type)
                    {
                    default:
                    {
                        printf("UDP: Unknown packet type\n");
                    }
                    break;
                    }
                }
            }
        }

        window_render();

        time_frame_end();
    }

    {
        struct data data = data_create(DATA_DISCONNECT_REQUEST);
        net_tcp_send(tcp_socket, &data, sizeof(data));
    }

    SDLNet_UDP_DelSocket(socket_set, udp_socket);
    SDLNet_TCP_DelSocket(socket_set, tcp_socket);
    SDLNet_FreeSocketSet(socket_set);
    net_udp_free_packet(udp_packet);
    SDLNet_UDP_Close(udp_socket);
    net_tcp_free_packet(tcp_packet);
    SDLNet_TCP_Close(tcp_socket);
    window_quit();
    engine_quit();

    return 0;
}
