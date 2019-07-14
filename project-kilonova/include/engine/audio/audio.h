#ifndef ENGINE_AUDIO_H
#define ENGINE_AUDIO_H

// Initializes the audio engine.
int audio_init(void);
void audio_set_listener(vec3 position, vec3 velocity, vec3 *orientation);
void audio_quit(void);

#endif
