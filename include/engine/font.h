#ifndef FONT_H
#define FONT_H

#include <SDL/SDL_ttf.h>

TTF_Font *font_open(const char *file, int pt);
SDL_Surface *font_render_solid(TTF_Font *font, const char *text, SDL_Color fg);
void font_close(TTF_Font *font);

#endif
