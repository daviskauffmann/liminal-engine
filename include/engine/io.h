#ifndef IO_H
#define IO_H

#include <SDL/SDL.h>
#include <stdbool.h>

const unsigned char *io_keyboard(int *num_keys);
unsigned int io_mouse(int *mouse_x, int *mouse_y);
int io_event(SDL_Event *event);
bool io_get_relative_mouse(void);
void io_set_relative_mouse(bool enabled);

#endif
