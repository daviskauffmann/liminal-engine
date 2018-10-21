#ifndef WINDOW_H
#define WINDOW_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

int window_init(const char *title, int width, int height);
const char *window_get_title(void);
void window_set_title(const char *title);
void window_get_size(int *width, int *height);
float window_get_aspect(void);
unsigned int window_get_flags(void);
void window_set_fullscreen(unsigned int flags);
void window_resize(int width, int height);
void window_swap(void);
void window_quit(void);

#endif
