#ifndef DATA_H
#define DATA_H

#define PACKET_SIZE 1024
#define MAX_STRLEN 256

enum data_type
{
    DATA_CONNECT_OK,
    DATA_CONNECT_FULL,
    DATA_CONNECT_BROADCAST,
    DATA_UDP_CONNECT_REQUEST,
    DATA_MOUSEDOWN_REQUEST,
    DATA_MOUSEDOWN_BROADCAST,
    DATA_CHAT_REQUEST,
    DATA_CHAT_BROADCAST,
    DATA_DISCONNECT_REQUEST,
    DATA_DISCONNECT_BROADCAST
};

struct data
{
    enum data_type type;
};

struct id_data
{
    struct data data;
    int id;
};

struct mouse_data
{
    struct data data;
    int id;
    int x;
    int y;
};

struct chat_data
{
    struct data data;
    int id;
    char message[MAX_STRLEN];
};

struct data data_create(enum data_type type);
struct id_data id_data_create(enum data_type type, int id);
struct mouse_data mouse_data_create(enum data_type type, int id, int x, int y);
struct chat_data chat_data_create(enum data_type type, int id, char *message);

#endif
