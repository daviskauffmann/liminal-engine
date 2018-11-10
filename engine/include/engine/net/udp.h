#ifndef UDP_H
#define UDP_H

UDPsocket ENGINE_API udp_open(unsigned short port);
UDPpacket ENGINE_API *udp_alloc_packet(int size);
int ENGINE_API udp_send(UDPsocket socket, UDPpacket *packet, IPaddress address, void *data, int len);
int ENGINE_API udp_recv(UDPsocket socket, UDPpacket *packet);
void ENGINE_API udp_free_packet(UDPpacket *packet);

#endif
