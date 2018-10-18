#ifndef WINDOW_H
#define WINDOW_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

extern struct program *basic_program;
extern struct program *phong_program;
extern struct texture *default_texture;
extern struct mesh *quad_mesh;
extern struct mesh *cube_mesh;
extern struct material *default_material;

int window_init(const char *title, int width, int height);
const char *window_get_title(void);
void window_set_title(const char *title);
void window_get_size(int *width, int *height);
float window_get_aspect(void);
unsigned int window_get_flags(void);
void window_toggle_fullscreen(void);
void window_resize(int width, int height);
void window_clear(void);
void window_swap(void);
void window_quit(void);

#endif
