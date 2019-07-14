#ifndef ENGINE_SOURCE_H
#define ENGINE_SOURCE_H

struct source
{
    ALuint source_id;
};

struct source *source_create(void);
void source_set_loop(struct source *source, bool loop);
void source_set_gain(struct source *source, float gain);
void source_set_pitch(struct source *source, float pitch);
void source_set_position(struct source *source, vec3 position);
void source_set_velocity(struct source *source, vec3 velocity);
bool source_is_playing(struct source *source);
void source_play(struct source *source, struct sound *sound);
void source_pause(struct source *source);
void source_resume(struct source *source);
void source_stop(struct source *source);
void source_destroy(struct source *source);

#endif
