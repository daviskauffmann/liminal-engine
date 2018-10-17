#include <engine/engine.h>

const unsigned char *io_keyboard(int *num_keys)
{
    return SDL_GetKeyboardState(num_keys);
}

unsigned int io_mouse(int *mouse_x, int *mouse_y)
{
    return SDL_GetMouseState(mouse_x, mouse_y);
}

int io_event(SDL_Event *event)
{
    return SDL_PollEvent(event);
}

bool io_get_relative_mouse(void)
{
    return SDL_GetRelativeMouseMode();
}

void io_set_relative_mouse(bool enabled)
{
    SDL_SetRelativeMouseMode(enabled);
}
