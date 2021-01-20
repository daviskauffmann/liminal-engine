#include <string.h>

#include "message.hpp"

pk::message::message(enum message_type type)
    : type(type)
{
}

pk::id_message::id_message(enum message_type type, int id)
    : message(type), id(id)
{
}

pk::connect_ok_message::connect_ok_message(enum message_type type, int id)
    : message(type), id(id)
{
}

pk::connect_broadcast_message::connect_broadcast_message(enum message_type type, int id, client_info client)
    : message(type), id(id), client(client)
{
}

pk::position_message::position_message(enum message_type type, float id, float x, float y, float z)
    : message(type), id(id), x(x), y(y), z(z)
{
}

pk::chat_message::chat_message(enum message_type type, int id, char *str)
    : message(type), id(id)
{
    strcpy(this->str, str);
}
