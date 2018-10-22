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
void broadcast(void *data, int len, int exclude_id);

static struct client clients[MAX_CLIENTS];

int main(int argc, char *args[])
{
    // setup engine
    if (core_init())
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

    if (SDLNet_ResolveHost(&server_address, INADDR_ANY, SERVER_PORT))
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
    UDPsocket udp_socket = udp_open(SERVER_PORT);

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
    SDLNet_SocketSet socket_set = SDLNet_AllocSocketSet(MAX_CLIENTS + 2);

    if (!socket_set)
    {
        error(SDLNet_GetError());

        return 1;
    }

    // add TCP and UDP sockets to socket set
    SDLNet_TCP_AddSocket(socket_set, tcp_socket);
    SDLNet_UDP_AddSocket(socket_set, udp_socket);

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
        // start of frame activities
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

                        info("Connected to client %s:%d", host, port);

                        // initialize the client
                        clients[client_id].id = client_id;
                        clients[client_id].socket = socket;

                        // add to the socket list
                        SDLNet_TCP_AddSocket(socket_set, clients[client_id].socket);

                        // send the client their info
                        {
                            struct id_data id_data = id_data_create(DATA_CONNECT_OK, clients[client_id].id);
                            tcp_send(socket, &id_data, sizeof(id_data));
                        }

                        // inform other clients
                        struct id_data id_data = id_data_create(DATA_CONNECT_BROADCAST, clients[client_id].id);
                        broadcast(&id_data, sizeof(id_data), clients[client_id].id);

                        // log the current number of clients
                        info("There are %d clients connected", count_clients());
                    }
                    else
                    {
                        info("A client tried to connect, but the server is full");

                        // send client a full server message
                        struct data data = data_create(DATA_CONNECT_FULL);
                        tcp_send(socket, &data, sizeof(data));
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
                        if (tcp_recv(clients[i].socket, tcp_packet) == 1)
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

                                info("Disconnecting from client %s:%d", host, port);

                                // inform other clients
                                struct id_data id_data = id_data_create(DATA_DISCONNECT_BROADCAST, clients[i].id);
                                broadcast(&id_data, sizeof(id_data), clients[i].id);

                                // close the TCP connection
                                SDLNet_TCP_DelSocket(socket_set, clients[i].socket);
                                SDLNet_TCP_Close(clients[i].socket);

                                // uninitialize the client
                                clients[i].id = -1;
                                clients[i].socket = NULL;

                                // log the current number of clients
                                info("There are %d clients connected", count_clients());
                            }
                            break;
                            case DATA_CHAT_REQUEST:
                            {
                                struct chat_data *chat_data = (struct chat_data *)data;

                                info("Client %d: %s", chat_data->id, chat_data->message);

                                // relay to other clients
                                struct chat_data chat_data2 = chat_data_create(DATA_CHAT_BROADCAST, chat_data->id, chat_data->message);
                                broadcast(&chat_data2, sizeof(chat_data2), clients[i].id);
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
                    case DATA_UDP_CONNECT_REQUEST:
                    {
                        struct id_data *id_data = (struct id_data *)data;

                        info("Saving UDP info of client %d", id_data->id);

                        // save the UDP address
                        clients[id_data->id].udp_address = udp_packet->address;
                    }
                    break;
                    case DATA_MOUSEDOWN_REQUEST:
                    {
                        struct mouse_data *mouse_data = (struct mouse_data *)data;

                        info("Client %d mouse down: (%d, %d)", mouse_data->id, mouse_data->x, mouse_data->y);
                    }
                    break;
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
    udp_free_packet(udp_packet);
    SDLNet_UDP_Close(udp_socket);
    tcp_free_packet(tcp_packet);
    SDLNet_TCP_Close(tcp_socket);

    // close engine
    net_quit();
    core_quit();

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

void broadcast(void *data, int len, int exclude_id)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].id != -1 && clients[i].id != exclude_id)
        {
            tcp_send(clients[i].socket, data, len);
        }
    }
}
