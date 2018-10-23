#include <engine/engine.h>
#include <shared/client.h>
#include <shared/data.h>

#define WINDOW_TITLE "Client"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 1000

#define FPS_CAP 30

int client_start(void)
{
    // setup engine
    if (core_init())
    {
        return 1;
    }

    if (window_init(
            WINDOW_TITLE,
            WINDOW_WIDTH,
            WINDOW_HEIGHT))
    {
        return 1;
    }

    if (net_init())
    {
        return 1;
    }

    time_cap_fps(FPS_CAP);

    // setup server info
    IPaddress server_address;

    if (SDLNet_ResolveHost(&server_address, SERVER_HOST, SERVER_PORT))
    {
        error(SDLNet_GetError());

        return 1;
    }

    // open TCP socket
    TCPsocket tcp_socket = tcp_open(&server_address);

    if (!tcp_socket)
    {
        return 1;
    }

    // allocate TCP packet
    TCPpacket *tcp_packet = tcp_alloc_packet(PACKET_SIZE);

    if (!tcp_packet)
    {
        return 1;
    }

    // open UDP socket
    UDPsocket udp_socket = udp_open(0);

    if (!udp_socket)
    {
        return 1;
    }

    // allocate UDP packet
    UDPpacket *udp_packet = udp_alloc_packet(PACKET_SIZE);

    if (!udp_packet)
    {
        return 1;
    }

    // allocate socket set
    SDLNet_SocketSet socket_set = SDLNet_AllocSocketSet(2);

    if (!socket_set)
    {
        error(SDLNet_GetError());

        return 1;
    }

    // add TCP and UDP sockets to socket set
    SDLNet_TCP_AddSocket(socket_set, tcp_socket);
    SDLNet_UDP_AddSocket(socket_set, udp_socket);

    // keep track of unique client ID given by server
    int client_id = -1;

    // check the server's response to the connection
    if (tcp_recv(tcp_socket, tcp_packet) == 1)
    {
        struct data *data = (struct data *)tcp_packet->data;

        switch (data->type)
        {
        case DATA_CONNECT_OK:
        {
            struct id_data *id_data = (struct id_data *)data;

            info("Server assigned ID: %d", id_data->id);

            client_id = id_data->id;
        }
        break;
        case DATA_CONNECT_FULL:
        {
            error("Server is full");

            return 1;
        }
        break;
        default:
        {
            error("Unknown server response");

            return 1;
        }
        break;
        }
    }

    // make a UDP "connection" to the server
    {
        struct id_data id_data = id_data_create(DATA_UDP_CONNECT_REQUEST, client_id);
        udp_send(udp_socket, udp_packet, server_address, &id_data, sizeof(id_data));
    }

    // main loop
    bool quit = false;
    while (!quit)
    {
        time_frame_start();

        // get keyboard input
        int num_keys;
        const unsigned char *keys = SDL_GetKeyboardState(&num_keys);

        // get mouse input
        int mouse_x, mouse_y;
        unsigned int mouse = SDL_GetMouseState(&mouse_x, &mouse_y);

        // handle events
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
                case SDLK_RETURN:
                {
                    struct chat_data chat_data = chat_data_create(DATA_CHAT_REQUEST, client_id, "Hello, World!");
                    tcp_send(tcp_socket, &chat_data, sizeof(chat_data));
                }
                break;
                }
            }
            break;
            case SDL_MOUSEBUTTONDOWN:
            {
                struct mouse_data mouse_data = mouse_data_create(DATA_MOUSEDOWN_REQUEST, client_id, event.button.x, event.button.y);
                udp_send(udp_socket, udp_packet, server_address, &mouse_data, sizeof(mouse_data));
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
                if (tcp_recv(tcp_socket, tcp_packet) == 1)
                {
                    struct data *data = (struct data *)tcp_packet->data;

                    switch (data->type)
                    {
                    case DATA_CONNECT_BROADCAST:
                    {
                        struct id_data *id_data = (struct id_data *)data;

                        info("Client with ID %d has joined", id_data->id);
                    }
                    break;
                    case DATA_DISCONNECT_BROADCAST:
                    {
                        struct id_data *id_data = (struct id_data *)data;

                        info("Client with ID %d has disconnected", id_data->id);
                    }
                    break;
                    case DATA_CHAT_BROADCAST:
                    {
                        struct chat_data *chat_data = (struct chat_data *)data;

                        info("Client %d: %s", chat_data->id, chat_data->message);
                    }
                    break;
                    default:
                    {
                        info("TCP: Unknown packet type");
                    }
                    break;
                    }
                }
            }

            // handle UDP messages
            if (SDLNet_SocketReady(udp_socket))
            {
                if (udp_recv(udp_socket, udp_packet) == 1)
                {
                    struct data *data = (struct data *)udp_packet->data;

                    switch (data->type)
                    {
                    default:
                    {
                        info("UDP: Unknown packet type");
                    }
                    break;
                    }
                }
            }
        }

        // end of frame activities
        time_frame_end();
    }

    // send a disconnect message
    {
        struct data data = data_create(DATA_DISCONNECT_REQUEST);
        tcp_send(tcp_socket, &data, sizeof(data));
    }

    // free resources
    SDLNet_UDP_DelSocket(socket_set, udp_socket);
    SDLNet_TCP_DelSocket(socket_set, tcp_socket);
    SDLNet_FreeSocketSet(socket_set);
    udp_free_packet(udp_packet);
    SDLNet_UDP_Close(udp_socket);
    tcp_free_packet(tcp_packet);
    SDLNet_TCP_Close(tcp_socket);

    // close engine
    image_quit();
    audio_quit();
    window_quit();
    core_quit();

    return 0;
}
