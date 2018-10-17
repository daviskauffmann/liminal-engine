#include <engine/engine.h>

#include "../shared/data.h"

#define WINDOW_TITLE "Multiplayer Test"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 1000

#define FPS_CAP 30

int main(int argc, char *argv[])
{
    // setup engine
    if (engine_init())
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    if (window_sw_init(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    time_cap_fps(FPS_CAP);

    // setup server info
    IPaddress server_address;

    if (net_resolve_host(&server_address, SERVER_HOST, SERVER_PORT))
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    // open TCP socket
    TCPsocket tcp_socket = net_tcp_open(&server_address);

    if (!tcp_socket)
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    // allocate TCP packet
    TCPpacket *tcp_packet = net_tcp_alloc_packet(PACKET_SIZE);

    if (!tcp_packet)
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    // open UDP socket
    UDPsocket udp_socket = net_udp_open(0);

    if (!udp_socket)
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    // allocate UDP packet
    UDPpacket *udp_packet = net_udp_alloc_packet(PACKET_SIZE);

    if (!udp_packet)
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    // allocate socket set
    SDLNet_SocketSet socket_set = net_alloc_socket_set(2);

    if (!socket_set)
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    // add TCP and UDP sockets to socket set
    net_tcp_add_socket(socket_set, tcp_socket);
    net_udp_add_socket(socket_set, udp_socket);

    // keep track of unique client ID given by server
    int client_id = -1;

    // check the server's response to the connection
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

    // make a UDP "connection" to the server
    {
        struct id_data id_data = id_data_create(DATA_UDP_CONNECT_REQUEST, client_id);
        net_udp_send(udp_socket, udp_packet, server_address, &id_data, sizeof(id_data));
    }

    // main loop
    bool quit = false;
    while (!quit)
    {
        time_frame_start();

        // get keyboard input
        int num_keys;
        const unsigned char *keys = io_keyboard(&num_keys);

        // get mouse input
        int mouse_x, mouse_y;
        unsigned int mouse = io_mouse(&mouse_x, &mouse_y);

        // handle events
        SDL_Event event;
        while (io_event(&event))
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
                    struct chat_data chat_data = chat_data_create(DATA_CHAT_REQUEST, client_id, "Hello, World!");
                    net_tcp_send(tcp_socket, &chat_data, sizeof(chat_data));
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
            }
        }

        // handle network events
        while (SDLNet_CheckSockets(socket_set, 0) > 0)
        {
            // handle TCP messages
            if (SDLNet_SocketReady(tcp_socket))
            {
                // accept new clients
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

            // handle UDP messages
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

        time_frame_end();
    }

    // send a disconnect message
    {
        struct data data = data_create(DATA_DISCONNECT_REQUEST);
        net_tcp_send(tcp_socket, &data, sizeof(data));
    }

    // free resources
    SDLNet_UDP_DelSocket(socket_set, udp_socket);
    SDLNet_TCP_DelSocket(socket_set, tcp_socket);
    SDLNet_FreeSocketSet(socket_set);
    net_udp_free_packet(udp_packet);
    SDLNet_UDP_Close(udp_socket);
    net_tcp_free_packet(tcp_packet);
    SDLNet_TCP_Close(tcp_socket);

    // close engine
    window_sw_quit();
    engine_quit();

    return 0;
}
