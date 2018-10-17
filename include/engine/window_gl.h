#ifndef WINDOW_GL_H
#define WINDOW_GL_H

#include <SDL/SDL.h>

int window_gl_init(const char *title, int width, int height);
const char *window_gl_get_title(void);
void window_gl_set_title(const char *title);
void window_gl_get_size(int *width, int *height);
float window_gl_get_aspect(void);
void window_gl_toggle_fullscreen(void);
void window_gl_resize(int width, int height);
void window_gl_clear(void);
void window_gl_render(void);
void window_gl_quit(void);

#endif
