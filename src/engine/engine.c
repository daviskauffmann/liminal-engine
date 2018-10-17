#include <engine/engine.h>

int engine_init(void)
{
    // init SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        error(SDL_GetError());

        return 1;
    }

    {
        SDL_version version;
        SDL_GetVersion(&version);

        info("SDL %d.%d.%d", version.major, version.minor, version.patch);
    }

    // init SDL_image
    {
        int flags = IMG_INIT_JPG | IMG_INIT_PNG;

        if (IMG_Init(flags) & flags != flags)
        {
            error(IMG_GetError());

            return 1;
        }
    }

    {
        const SDL_version *version = IMG_Linked_Version();

        info("SDL_image %d.%d.%d", version->major, version->minor, version->patch);
    }

    // init SDL_mixer
    {
        int flags = 0;

        if (Mix_Init(flags) & flags != flags)
        {
            error(Mix_GetError());

            return 1;
        }
    }

    {
        const SDL_version *version = Mix_Linked_Version();

        info("SDL_mixer %d.%d.%d", version->major, version->minor, version->patch);
    }

    // init SDL_net
    if (SDLNet_Init())
    {
        error(SDLNet_GetError());

        return 1;
    }

    {
        const SDL_version *version = SDLNet_Linked_Version();
        info("SDL_net %d.%d.%d", version->major, version->minor, version->patch);
    }

    // init SDL_ttf
    if (TTF_Init())
    {
        error(TTF_GetError());

        return 1;
    }

    {
        const SDL_version *version = TTF_Linked_Version();

        info("SDL_ttf %d.%d.%d", version->major, version->minor, version->patch);
    }

    return 0;
}

void engine_quit(void)
{
    TTF_Quit();
    SDLNet_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}
