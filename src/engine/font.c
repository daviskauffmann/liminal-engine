#include <engine/engine.h>

int font_init(void)
{
    // init SDL_ttf
    if (TTF_Init())
    {
        error(TTF_GetError());

        return 1;
    }

    // print version
    const SDL_version *version = TTF_Linked_Version();
    info("SDL_ttf %d.%d.%d", version->major, version->minor, version->patch);

    return 0;
}

void font_quit(void)
{
    TTF_Quit();
}
