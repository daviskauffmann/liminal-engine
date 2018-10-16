#include <SDL/SDL.h>
#include <shared/data.h>
#include <string.h>

struct data data_create(enum data_type type)
{
    struct data data;

    data.type = type;

    return data;
}

struct id_data id_data_create(enum data_type type, int id)
{
    struct id_data id_data;

    id_data.data = data_create(type);
    id_data.id = id;

    return id_data;
}

struct mouse_data mouse_data_create(enum data_type type, int id, int x, int y)
{
    struct mouse_data mouse_data;

    mouse_data.data = data_create(type);
    mouse_data.id = id;
    mouse_data.x = x;
    mouse_data.y = y;

    return mouse_data;
}

struct chat_data chat_data_create(enum data_type type, int id, char *message)
{
    struct chat_data chat_data;

    chat_data.data = data_create(type);
    chat_data.id = id;
    strcpy_s(chat_data.message, sizeof(chat_data.message), message);

    return chat_data;
}
