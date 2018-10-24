#include <engine/engine.h>

int image_init(void)
{
    // init SDL_image
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;

    if ((IMG_Init(flags) & flags) != flags)
    {
        error(IMG_GetError());

        return 1;
    }

    // print version
    const SDL_version *version = IMG_Linked_Version();
    info("SDL_image %d.%d.%d", version->major, version->minor, version->patch);

    return 0;
}

void image_quit(void)
{
    IMG_Quit();
}
