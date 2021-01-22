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

pk::position_message::position_message(enum message_type type, float id, float x, float y, float z, float x_rot, float y_rot, float z_rot)
    : message(type), id(id), x(x), y(y), z(z), x_rot(x_rot), y_rot(y_rot), z_rot(z_rot)
{
}

pk::chat_message::chat_message(enum message_type type, int id, char *str)
    : message(type), id(id)
{
    strcpy(this->str, str);
}
