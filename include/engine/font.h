#ifndef FONT_H
#define FONT_H

#include <SDL/SDL_ttf.h>

TTF_Font *font_open(const char *file, int pt);
void font_close(TTF_Font *font);

#endif
