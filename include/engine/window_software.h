#ifndef WINDOW_SOFTWARE_H
#define WINDOW_SOFTWARE_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

int window_software_init(const char *title, int width, int height);
const char *window_software_get_title(void);
void window_software_set_title(const char *title);
void window_software_toggle_fullscreen(void);
void window_software_clear(void);
void window_software_draw_pixels(SDL_Rect *rect, const void *pixels, int pitch);
void window_software_draw_text(TTF_Font *font, int px, int x, int y, SDL_Color fg, const char *const fmt, ...);
void window_software_render(void);
void window_software_quit(void);

#endif
