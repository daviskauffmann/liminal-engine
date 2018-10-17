#include <engine/engine.h>

int engine_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        error_set(SDL_GetError());

        return 1;
    }

    {
        SDL_version version;
        SDL_GetVersion(&version);
        printf("SDL %d.%d.%d.\n", version.major, version.minor, version.patch);
    }

    {
        int flags = IMG_INIT_JPG | IMG_INIT_PNG;

        if (IMG_Init(flags) & flags != flags)
        {
            error_set(IMG_GetError());

            return 1;
        }
    }

    {
        const SDL_version *version = IMG_Linked_Version();
        printf("SDL_image %d.%d.%d.\n", version->major, version->minor, version->patch);
    }

    {
        int flags = 0;

        if (Mix_Init(flags) & flags != flags)
        {
            error_set(Mix_GetError());

            return 1;
        }
    }

    {
        const SDL_version *version = Mix_Linked_Version();
        printf("SDL_mixer %d.%d.%d.\n", version->major, version->minor, version->patch);
    }

    if (SDLNet_Init())
    {
        error_set(SDLNet_GetError());

        return 1;
    }

    {
        const SDL_version *version = SDLNet_Linked_Version();
        printf("SDL_net %d.%d.%d.\n", version->major, version->minor, version->patch);
    }

    if (TTF_Init())
    {
        error_set(TTF_GetError());

        return 1;
    }

    {
        const SDL_version *version = TTF_Linked_Version();
        printf("SDL_ttf %d.%d.%d.\n", version->major, version->minor, version->patch);
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
