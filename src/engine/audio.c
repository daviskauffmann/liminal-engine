#include <engine/engine.h>

int audio_init(int frequency, unsigned short format, int channels, int chunk_size)
{
    if (Mix_OpenAudio(frequency, format, channels, chunk_size))
    {
        error(Mix_GetError());

        return 1;
    }

    return 0;
}

void audio_quit(void)
{
    Mix_CloseAudio();
}
