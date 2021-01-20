#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#define PACKET_SIZE 1024
#define MAX_STRLEN 256
#define MAX_CLIENTS 8

namespace pk
{
    enum message_type
    {
        MESSAGE_CONNECT_OK,
        MESSAGE_CONNECT_FULL,
        MESSAGE_CONNECT_BROADCAST,
        MESSAGE_UDP_CONNECT_REQUEST,
        MESSAGE_POSITION_REQUEST,
        MESSAGE_POSITION_BROADCAST,
        MESSAGE_CHAT_REQUEST,
        MESSAGE_CHAT_BROADCAST,
        MESSAGE_DISCONNECT_REQUEST,
        MESSAGE_DISCONNECT_BROADCAST
    };

    struct message
    {
        enum message_type type;

        message(enum message_type type);
    };

    struct id_message : public message
    {
        int id;

        id_message(enum message_type type, int id);
    };

    struct client_info
    {
        int id;
        float x;
        float y;
        float z;
    };

    struct connect_ok_message : public message
    {
        int id;
        client_info clients[MAX_CLIENTS];

        connect_ok_message(enum message_type type, int id);
    };

    struct connect_broadcast_message : public message
    {
        int id;
        client_info client;

        connect_broadcast_message(enum message_type type, int id, client_info client);
    };

    struct position_message : public message
    {
        int id;
        float x;
        float y;
        float z;

        position_message(enum message_type type, float id, float x, float y, float z);
    };

    struct chat_message : public message
    {
        int id;
        char str[MAX_STRLEN];

        chat_message(enum message_type type, int id, char *str);
    };
} // namespace pk

#endif