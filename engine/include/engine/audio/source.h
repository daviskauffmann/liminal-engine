#ifndef ENGINE_SOURCE_H
#define ENGINE_SOURCE_H

struct source
{
    ALuint source_id;
};

struct source ENGINE_API *source_create(void);
void ENGINE_API source_set_loop(struct source *source, bool loop);
void ENGINE_API source_set_gain(struct source *source, float gain);
void ENGINE_API source_set_pitch(struct source *source, float pitch);
void ENGINE_API source_set_position(struct source *source, vec3 position);
void ENGINE_API source_set_velocity(struct source *source, vec3 velocity);
bool ENGINE_API source_is_playing(struct source *source);
void ENGINE_API source_play(struct source *source, struct sound *sound);
void ENGINE_API source_pause(struct source *source);
void ENGINE_API source_resume(struct source *source);
void ENGINE_API source_stop(struct source *source);
void ENGINE_API source_destroy(struct source *source);

#endif
