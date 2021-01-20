#include "server.hpp"

#include <glm/vec3.hpp>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include "message.hpp"

#define SDL_FLAGS (0)

struct client
{
    int id;
    TCPsocket socket;
    IPaddress udp_address;
    glm::vec3 position;
};

static void broadcast_tcp(client *clients, void *data, int len, int exclude_id)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].id != -1 && clients[i].id != exclude_id)
        {
            if (SDLNet_TCP_Send(clients[i].socket, data, len) < len)
            {
                std::cerr << "Error: Failed to send TCP packet" << std::endl;
            }
        }
    }
}

static void broadcast_udp(UDPsocket socket, client *clients, void *data, int len, int exclude_id)
{
    UDPpacket *packet = SDLNet_AllocPacket(PACKET_SIZE);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].id != -1 && clients[i].id != exclude_id)
        {
            packet->address = clients[i].udp_address;
            packet->data = (unsigned char *)data;
            packet->len = len;

            if (SDLNet_UDP_Send(socket, -1, packet) != 1)
            {
                std::cerr << "Error: Failed to send UDP packet" << std::endl;
            }
        }
    }

    SDLNet_FreePacket(packet);
}

int pk::server_main(cxxopts::ParseResult result)
{
    if (SDL_Init(SDL_FLAGS) != 0)
    {
        std::cerr << "Error: Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (SDLNet_Init() != 0)
    {
        std::cerr << "Error: Failed to initialize SDL_net: " << SDLNet_GetError() << std::endl;
        return 1;
    }

    IPaddress server_address;
    unsigned short server_port = result["port"].as<unsigned short>();
    if (SDLNet_ResolveHost(&server_address, INADDR_ANY, server_port))
    {
        std::cerr << "Error: Failed to resolve host: " << SDLNet_GetError() << std::endl;
        return 1;
    }

    TCPsocket tcp_socket = SDLNet_TCP_Open(&server_address);
    if (!tcp_socket)
    {
        std::cerr << "Error: Failed to open TCP socket: {}" << SDLNet_GetError() << std::endl;
        return 1;
    }

    UDPsocket udp_socket = SDLNet_UDP_Open(server_port);
    if (!udp_socket)
    {
        std::cerr << "Error: Failed to open UDP socket: " << SDLNet_GetError() << std::endl;
        return 1;
    }

    SDLNet_SocketSet socket_set = SDLNet_AllocSocketSet(MAX_CLIENTS + 2);
    if (!socket_set)
    {
        std::cerr << "Error: Failed to allocate socket set: " << SDLNet_GetError() << std::endl;
        return 1;
    }
    SDLNet_TCP_AddSocket(socket_set, tcp_socket);
    SDLNet_UDP_AddSocket(socket_set, udp_socket);

    client clients[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i].id = -1;
        clients[i].position.x = 0;
        clients[i].position.y = 0;
        clients[i].position.z = 0;
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
                        std::cout << "Connected to client" << std::endl;

                        clients[client_id].id = client_id;
                        clients[client_id].socket = socket;

                        SDLNet_TCP_AddSocket(socket_set, clients[client_id].socket);

                        pk::connect_ok_message connect_ok_message = pk::connect_ok_message(pk::message_type::MESSAGE_CONNECT_OK, clients[client_id].id);
                        for (int i = 0; i < MAX_CLIENTS; i++)
                        {
                            connect_ok_message.clients[i].id = clients[i].id;

                            if (clients[i].id != -1)
                            {
                                connect_ok_message.clients[i].x = clients[i].position.x;
                                connect_ok_message.clients[i].y = clients[i].position.y;
                                connect_ok_message.clients[i].z = clients[i].position.z;
                            }
                        }
                        SDLNet_TCP_Send(socket, &connect_ok_message, sizeof(connect_ok_message));

                        client_info client;
                        client.x = clients[client_id].position.x;
                        client.y = clients[client_id].position.y;
                        client.z = clients[client_id].position.z;
                        pk::connect_broadcast_message connect_broadcast_message = pk::connect_broadcast_message(pk::message_type::MESSAGE_CONNECT_BROADCAST, clients[client_id].id, client);
                        broadcast_tcp(clients, &connect_broadcast_message, sizeof(connect_broadcast_message), clients[client_id].id);
                    }
                    else
                    {
                        std::cout << "A client tried to connect, but the server is full" << std::endl;

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
                                std::cout << "Client disconnected" << std::endl;

                                pk::id_message id_message = pk::id_message(pk::message_type::MESSAGE_DISCONNECT_BROADCAST, clients[i].id);
                                broadcast_tcp(clients, &id_message, sizeof(id_message), clients[i].id);

                                SDLNet_TCP_DelSocket(socket_set, clients[i].socket);
                                SDLNet_TCP_Close(clients[i].socket);

                                clients[i].id = -1;
                                clients[i].socket = nullptr;
                            }
                            break;
                            case pk::message_type::MESSAGE_CHAT_REQUEST:
                            {
                                pk::chat_message *chat_message = (pk::chat_message *)message;
                                std::cout << "Client " << chat_message->id << " says: " << chat_message->str << std::endl;

                                chat_message->type = MESSAGE_CHAT_BROADCAST;
                                broadcast_tcp(clients, chat_message, sizeof(*chat_message), clients[i].id);
                            }
                            break;
                            default:
                            {
                                std::cerr << "Error: Unknown TCP packet type: " << message->type << std::endl;
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

                        std::cout << "Saving UDP info of client " << id_message->id << std::endl;
                    }
                    break;
                    case pk::message_type::MESSAGE_POSITION_REQUEST:
                    {
                        pk::position_message *position_message = (pk::position_message *)message;

                        // TODO: validate position

                        pk::position_message *position_message2 = (pk::position_message *)malloc(sizeof(*position_message2));
                        position_message2->type = pk::message_type::MESSAGE_POSITION_BROADCAST;
                        position_message2->id = position_message->id;
                        position_message2->x = position_message->x;
                        position_message2->y = position_message->y;
                        position_message2->z = position_message->z;

                        broadcast_udp(udp_socket, clients, position_message2, sizeof(*position_message2), position_message2->id);
                    }
                    break;
                    default:
                    {
                        std::cerr << "Error: Unknown UDP packet type: " << message->type << std::endl;
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
