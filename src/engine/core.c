#include <engine/engine.h>

int core_init(void)
{
    // init SDL
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO))
    {
        error(SDL_GetError());

        return 1;
    }

    // print version
    SDL_version version;
    SDL_GetVersion(&version);
    info("SDL %d.%d.%d", version.major, version.minor, version.patch);

    return 0;
}

void core_quit(void)
{
    SDL_Quit();
}
