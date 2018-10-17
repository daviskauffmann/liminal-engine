#ifndef AUDIO_H
#define AUDIO_H

#include <SDL/SDL_mixer.h>
#include <stdbool.h>

int audio_init(int frequency, unsigned short format, int channels, int chunk_size);
void audio_quit(void);

#endif
