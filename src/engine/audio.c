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

Mix_Chunk *audio_chunk_load(const char *file)
{
    return Mix_LoadWAV(file);
}

void audio_chunk_play(int channel, Mix_Chunk *chunk, int loops)
{
    Mix_PlayChannel(channel, chunk, loops);
}

Mix_Music *audio_music_load(const char *file)
{
    return Mix_LoadMUS(file);
}

void audio_music_play(Mix_Music *music, int loops)
{
    Mix_PlayMusic(music, loops);
}

void audio_music_pause(void)
{
    Mix_PauseMusic();
}

void audio_music_resume(void)
{
    Mix_ResumeMusic();
}

void audio_music_stop(void)
{
    Mix_HaltMusic();
}

bool audio_music_playing(void)
{
    return Mix_PlayingMusic();
}

bool audio_music_paused(void)
{
    return Mix_PausedMusic();
}

void audio_music_free(Mix_Music *music)
{
    Mix_FreeMusic(music);
}

void audio_chunk_free(Mix_Chunk *chunk)
{
    Mix_FreeChunk(chunk);
}

void audio_quit(void)
{
    Mix_CloseAudio();
}
