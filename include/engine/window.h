#ifndef WINDOW_H
#define WINDOW_H

#include <SDL/SDL.h>

int window_init(const char *title, int width, int height);
const unsigned char *window_keyboard(int *num_keys);
unsigned int window_mouse(int *mouse_x, int *mouse_y);
int window_event(SDL_Event *event);
const char *window_get_title(void);
void window_set_title(const char *title);
void window_toggle_mouse(void);
void window_toggle_fullscreen(void);
void window_resize(int width, int height);
float window_aspect_ratio(void);
void window_clear(void);
void window_render(void);
void window_quit(void);

#endif
