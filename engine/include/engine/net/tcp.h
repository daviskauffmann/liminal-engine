#ifndef TCP_H
#define TCP_H

typedef struct
{
    Uint8 *data;
    int len;
    int maxlen;
} TCPpacket;

TCPsocket ENGINE_API tcp_open(IPaddress *address);
TCPpacket ENGINE_API *tcp_alloc_packet(int size);
int ENGINE_API tcp_send(TCPsocket socket, void *data, int len);
int ENGINE_API tcp_recv(TCPsocket socket, TCPpacket *packet);
void ENGINE_API tcp_free_packet(TCPpacket *packet);

#endif
