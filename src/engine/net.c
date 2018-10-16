#include <engine/error.h>
#include <engine/net.h>
#include <malloc.h>
#include <stdio.h>

int net_resolve_host(IPaddress *address, const char *host, unsigned short port)
{
    if (SDLNet_ResolveHost(address, host, port))
    {
        error_set(SDLNet_GetError());

        return 1;
    }

    return 0;
}

TCPsocket net_tcp_open(IPaddress *address)
{
    TCPsocket socket = SDLNet_TCP_Open(address);

    if (!socket)
    {
        error_set(SDLNet_GetError());

        return NULL;
    }

    const char *host = SDLNet_ResolveIP(address);
    unsigned short port = SDLNet_Read16(&address->port);
    printf("TCP: Open on %s:%i\n", host, port);

    return socket;
}

TCPpacket *net_tcp_alloc_packet(int size)
{
    TCPpacket *packet = malloc(sizeof(TCPpacket));

    if (!packet)
    {
        error_set("Couldn't allocate packet");

        return NULL;
    }

    packet->data = malloc(size);
    packet->maxlen = size;

    return packet;
}

void net_tcp_free_packet(TCPpacket *packet)
{
    free(packet->data);
    free(packet);
}

UDPsocket net_udp_open(unsigned short port)
{
    UDPsocket socket = SDLNet_UDP_Open(port);

    if (!socket)
    {
        error_set(SDLNet_GetError());

        return NULL;
    }

    printf("TCP: Open on port %i\n", port);

    return socket;
}

UDPpacket *net_udp_alloc_packet(int size)
{
    UDPpacket *packet = SDLNet_AllocPacket(size);

    if (!packet)
    {
        error_set(SDLNet_GetError());

        return NULL;
    }

    return packet;
}

void net_udp_free_packet(UDPpacket *packet)
{
    SDLNet_FreePacket(packet);
}

SDLNet_SocketSet net_alloc_socket_set(int max_sockets)
{
    SDLNet_SocketSet set = SDLNet_AllocSocketSet(2);

    if (!set)
    {
        error_set(SDLNet_GetError());

        return NULL;
    }

    return set;
}

int net_tcp_add_socket(SDLNet_SocketSet set, TCPsocket socket)
{
    return SDLNet_TCP_AddSocket(set, socket);
}

int net_udp_add_socket(SDLNet_SocketSet set, UDPsocket socket)
{
    return SDLNet_UDP_AddSocket(set, socket);
}

int net_tcp_send(TCPsocket socket, void *data, int len)
{
    IPaddress *address = SDLNet_TCP_GetPeerAddress(socket);
    const char *host = SDLNet_ResolveIP(address);
    unsigned short port = SDLNet_Read16(&address->port);

    printf("TCP: Sending %d bytes to %s:%d\n", len, host, port);

    return SDLNet_TCP_Send(socket, data, len);
}

int net_tcp_recv(TCPsocket socket, TCPpacket *packet)
{
    packet->len = SDLNet_TCP_Recv(socket, packet->data, packet->maxlen);

    if (packet->len > 0)
    {
        IPaddress *address = SDLNet_TCP_GetPeerAddress(socket);
        const char *host = SDLNet_ResolveIP(address);
        unsigned short port = SDLNet_Read16(&address->port);

        printf("TCP: Received %d bytes from %s:%d\n", packet->len, host, port);

        return 1;
    }

    return 0;
}

int net_udp_send(UDPsocket socket, UDPpacket *packet, IPaddress address, void *data, int len)
{
    packet->address = address;
    packet->data = (Uint8 *)data;
    packet->len = len;

    const char *host = SDLNet_ResolveIP(&packet->address);
    unsigned short port = SDLNet_Read16(&packet->address.port);

    printf("UDP: Sending %d bytes to %s:%d\n", packet->len, host, port);

    return SDLNet_UDP_Send(socket, -1, packet);
}

int net_udp_recv(UDPsocket socket, UDPpacket *packet)
{
    int recv = SDLNet_UDP_Recv(socket, packet);

    if (recv == 1)
    {
        const char *host = SDLNet_ResolveIP(&packet->address);
        unsigned short port = SDLNet_Read16(&packet->address.port);

        printf("UDP: Received %d bytes from %s:%d\n", packet->len, host, port);
    }

    return recv;
}
