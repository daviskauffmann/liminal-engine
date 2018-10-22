#include <engine/engine.h>

int audio_init(int frequency, unsigned short format, int channels, int chunk_size)
{
    // init SDL_mixer
    int flags = 0;

    if (Mix_Init(flags) & flags != flags)
    {
        error(Mix_GetError());

        return 1;
    }

    // setup audio
    if (Mix_OpenAudio(frequency, format, channels, chunk_size))
    {
        error(Mix_GetError());

        return 1;
    }

    // print version
    const SDL_version *version = Mix_Linked_Version();
    info("SDL_mixer %d.%d.%d", version->major, version->minor, version->patch);

    return 0;
}

void audio_quit(void)
{
    Mix_CloseAudio();
    Mix_Quit();
}
