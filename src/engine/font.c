#include <engine/engine.h>

TTF_Font *font_open(const char *file, int pt)
{
    return TTF_OpenFont(file, pt);
}

void font_close(TTF_Font *font)
{
    TTF_CloseFont(font);
}
