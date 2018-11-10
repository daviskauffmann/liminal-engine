#include <engine/engine.h>

int audio_init(int frequency, unsigned short format, int channels, int chunk_size)
{
    // setup audio
    if (Mix_OpenAudio(frequency, format, channels, chunk_size))
    {
        error(Mix_GetError());

        return 1;
    }

    return 0;
}

bool ENGINE_API audio_playing_music(void)
{
    return Mix_PlayingMusic();
}

bool ENGINE_API audio_paused_music(void)
{
    return Mix_PausedMusic();
}

void ENGINE_API audio_pause_music(void)
{
    Mix_PauseMusic();
}

void ENGINE_API audio_resume_music(void)
{
    Mix_ResumeMusic();
}

void ENGINE_API audio_stop_music(void)
{
    Mix_HaltMusic();
}

void audio_quit(void)
{
    Mix_CloseAudio();
}
