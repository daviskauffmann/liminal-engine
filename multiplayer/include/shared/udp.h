#ifndef UDP_H
#define UDP_H

UDPsocket udp_open(unsigned short port);
UDPpacket *udp_alloc_packet(int size);
int udp_send(UDPsocket socket, UDPpacket *packet, IPaddress address, void *data, int len);
int udp_recv(UDPsocket socket, UDPpacket *packet);
void udp_free_packet(UDPpacket *packet);

#endif
