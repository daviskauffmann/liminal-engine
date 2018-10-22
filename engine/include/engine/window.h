#ifndef WINDOW_H
#define WINDOW_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

int ENGINE_API window_init(const char *title, int width, int height);
const char ENGINE_API *window_get_title(void);
void ENGINE_API window_set_title(const char *title);
void ENGINE_API window_get_size(int *width, int *height);
float ENGINE_API window_get_aspect(void);
unsigned int ENGINE_API window_get_flags(void);
void ENGINE_API window_set_fullscreen(unsigned int flags);
void ENGINE_API window_resize(int width, int height);
void ENGINE_API window_swap(void);
void ENGINE_API window_quit(void);

#endif
