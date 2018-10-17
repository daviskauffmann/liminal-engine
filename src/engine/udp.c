#include <engine/engine.h>

UDPsocket udp_open(unsigned short port)
{
    UDPsocket socket = SDLNet_UDP_Open(port);

    if (!socket)
    {
        error(SDLNet_GetError());

        return NULL;
    }

    info("TCP: Open on port %i", port);

    return socket;
}

UDPpacket *udp_alloc_packet(int size)
{
    UDPpacket *packet = SDLNet_AllocPacket(size);

    if (!packet)
    {
        error(SDLNet_GetError());

        return NULL;
    }

    return packet;
}

int udp_send(UDPsocket socket, UDPpacket *packet, IPaddress address, void *data, int len)
{
    packet->address = address;
    packet->data = (Uint8 *)data;
    packet->len = len;

    const char *host = SDLNet_ResolveIP(&packet->address);
    unsigned short port = SDLNet_Read16(&packet->address.port);

    info("UDP: Sending %d bytes to %s:%d", packet->len, host, port);

    return SDLNet_UDP_Send(socket, -1, packet);
}

int udp_recv(UDPsocket socket, UDPpacket *packet)
{
    int recv = SDLNet_UDP_Recv(socket, packet);

    if (recv == 1)
    {
        const char *host = SDLNet_ResolveIP(&packet->address);
        unsigned short port = SDLNet_Read16(&packet->address.port);

        info("UDP: Received %d bytes from %s:%d", packet->len, host, port);
    }

    return recv;
}

void udp_free_packet(UDPpacket *packet)
{
    SDLNet_FreePacket(packet);
}
