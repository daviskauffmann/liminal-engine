#ifndef NET_H
#define NET_H

#include <SDL/SDL_net.h>

typedef struct
{
    Uint8 *data;
    int len;
    int maxlen;
} TCPpacket;

int net_resolve_host(IPaddress *address, const char *host, unsigned short port);
TCPsocket net_tcp_open(IPaddress *address);
TCPpacket *net_tcp_alloc_packet(int size);
void net_tcp_free_packet(TCPpacket *packet);
UDPsocket net_udp_open(unsigned short port);
UDPpacket *net_udp_alloc_packet(int size);
void net_udp_free_packet(UDPpacket *packet);
SDLNet_SocketSet net_alloc_socket_set(int max_sockets);
int net_tcp_add_socket(SDLNet_SocketSet set, TCPsocket socket);
int net_udp_add_socket(SDLNet_SocketSet set, UDPsocket socket);
int net_tcp_send(TCPsocket socket, void *data, int len);
int net_tcp_recv(TCPsocket socket, TCPpacket *packet);
int net_udp_send(UDPsocket socket, UDPpacket *packet, IPaddress address, void *data, int len);
int net_udp_recv(UDPsocket socket, UDPpacket *packet);

#endif
