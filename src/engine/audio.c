#include <engine/engine.h>

int audio_init(void)
{
    if (Mix_OpenAudio(
            MIX_DEFAULT_FREQUENCY,
            MIX_DEFAULT_FORMAT,
            MIX_DEFAULT_CHANNELS,
            1024))
    {
        error_set(Mix_GetError());

        return 1;
    }

    return 0;
}

void audio_quit(void)
{
    Mix_CloseAudio();
}
