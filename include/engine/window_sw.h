#ifndef WINDOW_SOFTWARE_H
#define WINDOW_SOFTWARE_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

int window_sw_init(const char *title, int width, int height);
const char *window_sw_get_title(void);
void window_sw_set_title(const char *title);
void window_sw_get_size(int *width, int *height);
float window_sw_get_aspect(void);
void window_sw_toggle_fullscreen(void);
void window_sw_clear(void);
void window_sw_draw_pixels(SDL_Rect *rect, const void *pixels, int pitch);
void window_sw_draw_text(TTF_Font *font, int px, int x, int y, SDL_Color fg, const char *const fmt, ...);
void window_sw_render(void);
void window_sw_quit(void);

#endif
