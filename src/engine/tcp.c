#include <engine/engine.h>

TCPsocket tcp_open(IPaddress *address)
{
    TCPsocket socket = SDLNet_TCP_Open(address);

    if (!socket)
    {
        error(SDLNet_GetError());

        return NULL;
    }

    {
        const char *host = SDLNet_ResolveIP(address);
        unsigned short port = SDLNet_Read16(&address->port);

        info("TCP: Open on %s:%i", host, port);
    }

    return socket;
}

TCPpacket *tcp_alloc_packet(int size)
{
    TCPpacket *packet = malloc(sizeof(TCPpacket));

    if (!packet)
    {
        error("Couldn't allocate packet");

        return NULL;
    }

    packet->data = malloc(size);
    packet->maxlen = size;

    return packet;
}

int tcp_send(TCPsocket socket, void *data, int len)
{
    IPaddress *address = SDLNet_TCP_GetPeerAddress(socket);
    const char *host = SDLNet_ResolveIP(address);
    unsigned short port = SDLNet_Read16(&address->port);

    info("TCP: Sending %d bytes to %s:%d", len, host, port);

    return SDLNet_TCP_Send(socket, data, len);
}

int tcp_recv(TCPsocket socket, TCPpacket *packet)
{
    packet->len = SDLNet_TCP_Recv(socket, packet->data, packet->maxlen);

    if (packet->len > 0)
    {
        IPaddress *address = SDLNet_TCP_GetPeerAddress(socket);
        const char *host = SDLNet_ResolveIP(address);
        unsigned short port = SDLNet_Read16(&address->port);

        info("TCP: Received %d bytes from %s:%d", packet->len, host, port);

        return 1;
    }

    return 0;
}

void tcp_free_packet(TCPpacket *packet)
{
    free(packet->data);
    free(packet);
}
