#ifndef AUDIO_H
#define AUDIO_H

// Initializes the audio engine.
int ENGINE_API audio_init(void);
void ENGINE_API audio_set_listener(vec3 position, vec3 velocity, vec3 *orientation);
void ENGINE_API audio_quit(void);

#endif
