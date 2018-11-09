#include <engine/engine.h>

#define SDL_FLAGS (SDL_INIT_AUDIO | SDL_INIT_VIDEO)
#define IMG_FLAGS (IMG_INIT_JPG | IMG_INIT_PNG)
#define MIX_FLAGS 0

int core_init(void)
{
    // init SDL
    if (SDL_Init(SDL_FLAGS))
    {
        error(SDL_GetError());

        return 1;
    }

    // print version
    {
        SDL_version version;
        SDL_GetVersion(&version);
        info("SDL %d.%d.%d", version.major, version.minor, version.patch);
    }

    // init SDL_image
    if ((IMG_Init(IMG_FLAGS) & IMG_FLAGS) != IMG_FLAGS)
    {
        error(IMG_GetError());

        return 1;
    }

    // print version
    {
        const SDL_version *version = IMG_Linked_Version();
        info("SDL_image %d.%d.%d", version->major, version->minor, version->patch);
    }

    // init SDL_mixer
    if ((Mix_Init(MIX_FLAGS) & MIX_FLAGS) != MIX_FLAGS)
    {
        error(Mix_GetError());

        return 1;
    }

    // print version
    {
        const SDL_version *version = Mix_Linked_Version();
        info("SDL_mixer %d.%d.%d", version->major, version->minor, version->patch);
    }

    // init SDL_ttf
    if (TTF_Init())
    {
        error(TTF_GetError());

        return 1;
    }

    // print version
    {
        const SDL_version *version = TTF_Linked_Version();
        info("SDL_ttf %d.%d.%d", version->major, version->minor, version->patch);
    }

    // init SDL_net
    if (SDLNet_Init())
    {
        error(SDLNet_GetError());

        return 1;
    }

    // print version
    {
        const SDL_version *version = SDLNet_Linked_Version();
        info("SDL_net %d.%d.%d", version->major, version->minor, version->patch);
    }

    return 0;
}

void core_quit(void)
{
    SDLNet_Quit();
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}
