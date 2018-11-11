#ifndef AUDIO_H
#define AUDIO_H

int ENGINE_API audio_init(int frequency, unsigned short format, int channels, int chunk_size);
void ENGINE_API audio_set_listener(vec3 position);
void ENGINE_API audio_quit(void);

#endif
