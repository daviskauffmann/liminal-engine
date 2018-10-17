#include <engine/engine.h>

TTF_Font *font_open(const char *file, int pt)
{
    return TTF_OpenFont(file, pt);
}

SDL_Surface *font_render_solid(TTF_Font *font, const char *text, SDL_Color fg)
{
    return TTF_RenderText_Solid(font, text, fg);
}

void font_close(TTF_Font *font)
{
    TTF_CloseFont(font);
}
