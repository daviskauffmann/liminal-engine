#ifndef GAME_AUDIO_H
#define GAME_AUDIO_H

int audio_init(void);
void audio_set_listener(vec3 position, vec3 velocity, vec3 *orientation);
void audio_quit(void);

#endif
