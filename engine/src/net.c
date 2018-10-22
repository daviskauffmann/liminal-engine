#include <engine/engine.h>

int net_init(void)
{
    // init SDL_net
    if (SDLNet_Init())
    {
        error(SDLNet_GetError());

        return 1;
    }

    // print version
    const SDL_version *version = SDLNet_Linked_Version();
    info("SDL_net %d.%d.%d", version->major, version->minor, version->patch);

    return 0;
}

void net_quit(void)
{
    SDLNet_Quit();
}
