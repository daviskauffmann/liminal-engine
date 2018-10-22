#ifndef AUDIO_H
#define AUDIO_H

#include <SDL/SDL_mixer.h>
#include <stdbool.h>

int ENGINE_API audio_init(int frequency, unsigned short format, int channels, int chunk_size);
void ENGINE_API audio_quit(void);

#endif
