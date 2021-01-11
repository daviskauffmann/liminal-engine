#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#define PACKET_SIZE 1024
#define MAX_STRLEN 256

namespace pk
{
    enum message_type
    {
        MESSAGE_CONNECT_OK,
        MESSAGE_CONNECT_FULL,
        MESSAGE_CONNECT_BROADCAST,
        MESSAGE_UDP_CONNECT_REQUEST,
        MESSAGE_MOUSEDOWN_REQUEST,
        MESSAGE_MOUSEDOWN_BROADCAST,
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

    struct mouse_message : public message
    {
        int id;
        int x;
        int y;

        mouse_message(enum message_type type, int id, int x, int y);
    };

    struct chat_message : public message
    {
        int id;
        char str[MAX_STRLEN];

        chat_message(enum message_type type, int id, char *str);
    };
} // namespace pk

#endif
