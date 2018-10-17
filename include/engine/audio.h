#ifndef AUDIO_H
#define AUDIO_H

#include <SDL/SDL_mixer.h>
#include <stdbool.h>

int audio_init(void);
Mix_Chunk *audio_load_chunk(const char *file);
void audio_play_chunk(int channel, Mix_Chunk *chunk, int loops);
Mix_Music *audio_load_music(const char *file);
void audio_play_music(Mix_Music *music, int loops);
void audio_pause_music(void);
void audio_resume_music(void);
void audio_stop_music(void);
bool audio_playing_music(void);
bool audio_paused_music(void);
void audio_free_music(Mix_Music *music);
void audio_free_chunk(Mix_Chunk *chunk);
void audio_quit(void);

#endif
