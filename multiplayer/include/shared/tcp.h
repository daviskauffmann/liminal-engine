#ifndef TCP_H
#define TCP_H

typedef struct
{
    Uint8 *data;
    int len;
    int maxlen;
} TCPpacket;

TCPsocket tcp_open(IPaddress *address);
TCPpacket *tcp_alloc_packet(int size);
int tcp_send(TCPsocket socket, void *data, int len);
int tcp_recv(TCPsocket socket, TCPpacket *packet);
void tcp_free_packet(TCPpacket *packet);

#endif
