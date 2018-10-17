#ifndef WINDOW_OPENGL_H
#define WINDOW_OPENGL_H

#include <SDL/SDL.h>

int window_opengl_init(const char *title, int width, int height);
const char *window_opengl_get_title(void);
void window_opengl_set_title(const char *title);
void window_opengl_toggle_fullscreen(void);
void window_opengl_resize(int width, int height);
float window_opengl_aspect_ratio(void);
void window_opengl_clear(void);
void window_opengl_render(void);
void window_opengl_quit(void);

#endif
