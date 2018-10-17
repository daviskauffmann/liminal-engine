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

Mix_Chunk *audio_load_chunk(const char *file)
{
    return Mix_LoadWAV(file);
}

void audio_play_chunk(int channel, Mix_Chunk *chunk, int loops)
{
    Mix_PlayChannel(channel, chunk, loops);
}

Mix_Music *audio_load_music(const char *file)
{
    return Mix_LoadMUS(file);
}

void audio_play_music(Mix_Music *music, int loops)
{
    Mix_PlayMusic(music, loops);
}

void audio_pause_music(void)
{
    Mix_PauseMusic();
}

void audio_resume_music(void)
{
    Mix_ResumeMusic();
}

void audio_stop_music(void)
{
    Mix_HaltMusic();
}

bool audio_playing_music(void)
{
    return Mix_PlayingMusic();
}

bool audio_paused_music(void)
{
    return Mix_PausedMusic();
}

void audio_free_music(Mix_Music *music)
{
    Mix_FreeMusic(music);
}

void audio_free_chunk(Mix_Chunk *chunk)
{
    Mix_FreeChunk(chunk);
}

void audio_quit(void)
{
    Mix_CloseAudio();
}
