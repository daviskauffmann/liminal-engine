#include <engine/engine.h>

#include "../shared/data.h"

#define SERVER_PORT 1000
#define MAX_CLIENTS 8

#define FPS_CAP 30

// TODO: handle timeouts on clients to automatically disconnect them
struct client
{
    int id;
    TCPsocket socket;
    IPaddress udp_address;
};

int count_clients(void);

static struct client clients[MAX_CLIENTS];

int main(int argc, char *args[])
{
    // setup engine
    if (engine_init())
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    time_cap_fps(FPS_CAP);

    // setup server info
    IPaddress server_address;

    if (net_resolve_host(&server_address, INADDR_ANY, SERVER_PORT))
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
    UDPsocket udp_socket = net_udp_open(SERVER_PORT);

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
    SDLNet_SocketSet socket_set = net_alloc_socket_set(MAX_CLIENTS + 2);

    if (!socket_set)
    {
        printf("Error: %s\n", error_get());

        return 1;
    }

    // add TCP and UDP sockets to socket set
    net_tcp_add_socket(socket_set, tcp_socket);
    net_udp_add_socket(socket_set, udp_socket);

    // setup client list
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i].id = -1;
        clients[i].socket = NULL;
    }

    // main loop
    bool quit = false;
    while (!quit)
    {
        time_frame_start();

        // handle network events
        while (SDLNet_CheckSockets(socket_set, 0) > 0)
        {
            // check activity on the server
            if (SDLNet_SocketReady(tcp_socket))
            {
                // accept new clients
                TCPsocket socket = SDLNet_TCP_Accept(tcp_socket);

                if (socket)
                {
                    // search for an empty client
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
                        // get socket info
                        IPaddress *address = SDLNet_TCP_GetPeerAddress(socket);
                        const char *host = SDLNet_ResolveIP(address);
                        unsigned short port = SDLNet_Read16(&address->port);

                        printf("Connected to client %s:%d\n", host, port);

                        // initialize the client
                        clients[client_id].id = client_id;
                        clients[client_id].socket = socket;

                        // add to the socket list
                        net_tcp_add_socket(socket_set, clients[client_id].socket);

                        // send the client their info
                        {
                            struct id_data id_data = id_data_create(DATA_CONNECT_OK, clients[client_id].id);
                            net_tcp_send(socket, &id_data, sizeof(id_data));
                        }

                        // inform other clients
                        for (int i = 0; i < MAX_CLIENTS; i++)
                        {
                            if (clients[i].id != -1 && clients[i].id != clients[client_id].id)
                            {
                                struct id_data id_data = id_data_create(DATA_CONNECT_BROADCAST, clients[client_id].id);
                                net_tcp_send(clients[i].socket, &id_data, sizeof(id_data));
                            }
                        }

                        // log the current number of clients
                        printf("There are %d clients connected\n", count_clients());
                    }
                    else
                    {
                        printf("A client tried to connect, but the server is full\n");

                        // send client a full server message
                        struct data data = data_create(DATA_CONNECT_FULL);
                        net_tcp_send(socket, &data, sizeof(data));
                    }
                }
            }

            // check activity on each client
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (clients[i].id != -1)
                {
                    // handle TCP messages
                    if (SDLNet_SocketReady(clients[i].socket))
                    {
                        if (net_tcp_recv(clients[i].socket, tcp_packet) == 1)
                        {
                            struct data *data = (struct data *)tcp_packet->data;

                            switch (data->type)
                            {
                            case DATA_DISCONNECT_REQUEST:
                            {
                                // get socket info
                                IPaddress *address = SDLNet_TCP_GetPeerAddress(clients[i].socket);
                                const char *host = SDLNet_ResolveIP(address);
                                unsigned short port = SDLNet_Read16(&address->port);

                                printf("Disconnecting from client %s:%d\n", host, port);

                                // inform other clients
                                for (int j = 0; j < MAX_CLIENTS; j++)
                                {
                                    if (clients[j].id != -1 && clients[j].id != clients[i].id)
                                    {
                                        struct id_data id_data = id_data_create(DATA_DISCONNECT_BROADCAST, clients[i].id);
                                        net_tcp_send(clients[j].socket, &id_data, sizeof(id_data));
                                    }
                                }

                                // close the TCP connection
                                SDLNet_TCP_DelSocket(socket_set, clients[i].socket);
                                SDLNet_TCP_Close(clients[i].socket);

                                // uninitialize the client
                                clients[i].id = -1;
                                clients[i].socket = NULL;

                                // log the current number of clients
                                printf("There are %d clients connected\n", count_clients());
                            }
                            break;
                            case DATA_CHAT_REQUEST:
                            {
                                struct chat_data *chat_data = (struct chat_data *)data;

                                printf("Client %d: %s\n", chat_data->id, chat_data->message);

                                // relay to other clients
                                for (int j = 0; j < MAX_CLIENTS; j++)
                                {
                                    if (clients[j].id != -1 && clients[j].id != clients[i].id)
                                    {
                                        struct chat_data chat_data2 = chat_data_create(DATA_CHAT_BROADCAST, chat_data->id, chat_data->message);
                                        net_tcp_send(clients[j].socket, &chat_data2, sizeof(chat_data2));
                                    }
                                }
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
                    case DATA_UDP_CONNECT_REQUEST:
                    {
                        struct id_data *id_data = (struct id_data *)data;

                        printf("Saving UDP info of client %d\n", id_data->id);

                        // save the UDP address
                        clients[id_data->id].udp_address = udp_packet->address;
                    }
                    break;
                    case DATA_MOUSEDOWN_REQUEST:
                    {
                        struct mouse_data *mouse_data = (struct mouse_data *)data;

                        printf("Client %d mouse down: (%d, %d)\n", mouse_data->id, mouse_data->x, mouse_data->y);
                    }
                    break;
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

    // close clients
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

    // free resources
    SDLNet_UDP_DelSocket(socket_set, udp_socket);
    SDLNet_TCP_DelSocket(socket_set, tcp_socket);
    SDLNet_FreeSocketSet(socket_set);
    net_udp_free_packet(udp_packet);
    SDLNet_UDP_Close(udp_socket);
    net_tcp_free_packet(tcp_packet);
    SDLNet_TCP_Close(tcp_socket);

    // close engine
    engine_quit();

    return 0;
}

int count_clients(void)
{
    int num_clients = 0;

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].id != -1)
        {
            num_clients++;
        }
    }

    return num_clients;
}
