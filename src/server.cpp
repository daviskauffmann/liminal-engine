#include "server.hpp"

#include <cxxopts.hpp>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <spdlog/spdlog.h>

#include "message.hpp"

#define SDL_FLAGS (0)

#define WINDOW_TITLE "Project Kilonova"

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 3000
#define MAX_CLIENTS 8

struct client
{
    int id;
    TCPsocket socket;
    IPaddress udp_address;
};

static struct client clients[MAX_CLIENTS];

static void broadcast(void *data, int len, int exclude_id)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].id != -1 && clients[i].id != exclude_id)
        {
            SDLNet_TCP_Send(clients[i].socket, data, len);
        }
    }
}

int pk::server_main(cxxopts::ParseResult result)
{
    if (SDL_Init(SDL_FLAGS) != 0)
    {
        spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
        return 1;
    }

    if (SDLNet_Init() != 0)
    {
        spdlog::error("Failed to initialize SDL_net: {}", SDLNet_GetError());
        return 1;
    }

    IPaddress server_address;
    if (SDLNet_ResolveHost(&server_address, INADDR_ANY, SERVER_PORT))
    {
        spdlog::error("Failed to resolve host: {}", SDLNet_GetError());
        return 1;
    }

    TCPsocket tcp_socket = SDLNet_TCP_Open(&server_address);
    if (!tcp_socket)
    {
        spdlog::error("Failed to open TCP socket: {}", SDLNet_GetError());
        return 1;
    }

    UDPsocket udp_socket = SDLNet_UDP_Open(SERVER_PORT);
    if (!udp_socket)
    {
        spdlog::error("Failed to open UDP socket: {}", SDLNet_GetError());
        return 1;
    }

    SDLNet_SocketSet socket_set = SDLNet_AllocSocketSet(MAX_CLIENTS + 2);
    if (!socket_set)
    {
        spdlog::error("Failed to allocate socket set: {}", SDLNet_GetError());
        return 1;
    }
    SDLNet_TCP_AddSocket(socket_set, tcp_socket);
    SDLNet_UDP_AddSocket(socket_set, udp_socket);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i].id = -1;
        clients[i].socket = NULL;
    }

    // unsigned int current_time = 0;
    // float time_scale = 1.0f;

    bool quit = false;
    while (!quit)
    {
        // unsigned int previous_time = current_time;
        // current_time = SDL_GetTicks();
        // float delta_time = ((current_time - previous_time) / 1000.0f) * time_scale;

        while (SDLNet_CheckSockets(socket_set, 0) > 0)
        {
            if (SDLNet_SocketReady(tcp_socket))
            {
                TCPsocket socket = SDLNet_TCP_Accept(tcp_socket);
                if (socket)
                {
                    int client_id = -1;
                    for (int i = 0; i < MAX_CLIENTS; i++)
                    {
                        if (clients[i].id == -1)
                        {
                            client_id = i;
                            break;
                        }
                    }
                    if (client_id != -1)
                    {
                        spdlog::info("Connected to client");

                        clients[client_id].id = client_id;
                        clients[client_id].socket = socket;

                        SDLNet_TCP_AddSocket(socket_set, clients[client_id].socket);

                        {
                            pk::id_message id_message = pk::id_message(pk::message_type::MESSAGE_CONNECT_OK, clients[client_id].id);
                            SDLNet_TCP_Send(socket, &id_message, sizeof(id_message));
                        }

                        pk::id_message id_message = pk::id_message(pk::message_type::MESSAGE_CONNECT_BROADCAST, clients[client_id].id);
                        broadcast(&id_message, sizeof(id_message), clients[client_id].id);
                    }
                    else
                    {
                        spdlog::info("A client tried to connect, but the server is full");

                        pk::message message = pk::message(pk::message_type::MESSAGE_CONNECT_FULL);
                        SDLNet_TCP_Send(socket, &message, sizeof(message));
                    }
                }
            }

            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (clients[i].id != -1)
                {
                    if (SDLNet_SocketReady(clients[i].socket))
                    {
                        char buffer[PACKET_SIZE];
                        if (SDLNet_TCP_Recv(clients[i].socket, buffer, sizeof(buffer)) > 1)
                        {
                            pk::message *message = (pk::message *)buffer;
                            switch (message->type)
                            {
                            case pk::message_type::MESSAGE_DISCONNECT_REQUEST:
                            {
                                spdlog::info("Client disconnected");

                                pk::id_message id_message = pk::id_message(pk::message_type::MESSAGE_DISCONNECT_BROADCAST, clients[i].id);
                                broadcast(&id_message, sizeof(id_message), clients[i].id);

                                SDLNet_TCP_DelSocket(socket_set, clients[i].socket);
                                SDLNet_TCP_Close(clients[i].socket);

                                clients[i].id = -1;
                                clients[i].socket = NULL;
                            }
                            break;
                            case pk::message_type::MESSAGE_CHAT_REQUEST:
                            {
                                pk::chat_message *chat_message = (pk::chat_message *)message;
                                spdlog::info("Client {}: {}", chat_message->id, chat_message->str);

                                pk::chat_message chat_message2 = pk::chat_message(pk::message_type::MESSAGE_CHAT_BROADCAST, chat_message->id, chat_message->str);
                                broadcast(&chat_message2, sizeof(chat_message2), clients[i].id);
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
                    case pk::message_type::MESSAGE_UDP_CONNECT_REQUEST:
                    {
                        pk::id_message *id_message = (pk::id_message *)message;
                        clients[id_message->id].udp_address = packet->address;

                        spdlog::info("Saving UDP info of client {}", id_message->id);
                    }
                    break;
                    case pk::message_type::MESSAGE_MOUSEDOWN_REQUEST:
                    {
                        pk::mouse_message *mouse_message = (pk::mouse_message *)message;

                        spdlog::info("Client {} mouse down: ({}, {})", mouse_message->id, mouse_message->x, mouse_message->y);
                    }
                    break;
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

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].id != -1)
        {
            SDLNet_TCP_DelSocket(socket_set, clients[i].socket);
            SDLNet_TCP_Close(clients[i].socket);

            clients[i].id = -1;
            clients[i].socket = NULL;
        }
    }

    SDLNet_UDP_DelSocket(socket_set, udp_socket);
    SDLNet_TCP_DelSocket(socket_set, tcp_socket);

    SDLNet_FreeSocketSet(socket_set);

    SDLNet_UDP_Close(udp_socket);
    SDLNet_TCP_Close(tcp_socket);

    SDLNet_Quit();

    SDL_Quit();

    return 0;
}
