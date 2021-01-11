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

pk::mouse_message::mouse_message(enum message_type type, int id, int x, int y)
    : message(type), id(id), x(x), y(y)
{
}

pk::chat_message::chat_message(enum message_type type, int id, char *str)
    : message(type), id(id)
{
    strcpy(this->str, str);
}
