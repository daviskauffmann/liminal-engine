#ifndef AUDIO_H
#define AUDIO_H

#include <SDL/SDL_mixer.h>
#include <stdbool.h>

int audio_init(void);

Mix_Chunk *audio_chunk_load(const char *file);
void audio_chunk_play(int channel, Mix_Chunk *chunk, int loops);
void audio_chunk_free(Mix_Chunk *chunk);

Mix_Music *audio_music_load(const char *file);
void audio_music_play(Mix_Music *music, int loops);
void audio_music_pause(void);
void audio_music_resume(void);
void audio_music_stop(void);
bool audio_music_playing(void);
bool audio_music_paused(void);
void audio_music_free(Mix_Music *music);

void audio_quit(void);

#endif
