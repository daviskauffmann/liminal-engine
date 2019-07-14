#include <engine/engine.h>

struct source *source_create(void)
{
    struct source *source = malloc(sizeof(struct source));

    if (!source)
    {
        printf("Error: Couldn't allocate source\n");

        return NULL;
    }

    alGenSources(1, &source->source_id);


    return source;
}

void source_set_loop(struct source *source, bool loop)
{
    alSourcei(source->source_id, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

void source_set_gain(struct source *source, float gain)
{
    alSourcef(source->source_id, AL_GAIN, gain);
}

void source_set_pitch(struct source *source, float pitch)
{
    alSourcef(source->source_id, AL_PITCH, pitch);
}

void source_set_position(struct source *source, vec3 position)
{
    alSourcefv(source->source_id, AL_POSITION, (ALfloat *)position);
}

void source_set_velocity(struct source *source, vec3 velocity)
{
    alSourcefv(source->source_id, AL_VELOCITY, (ALfloat *)velocity);
}

bool source_is_playing(struct source *source)
{
    ALint value;
    alGetSourcei(source->source_id, AL_SOURCE_STATE, &value);

    return value == AL_PLAYING;
}

void source_play(struct source *source, struct sound *sound)
{
    alSourcei(source->source_id, AL_BUFFER, sound->buffer_id);
    alSourcePlay(source->source_id);
}

void source_resume(struct source *source)
{
    alSourcePlay(source->source_id);
}

void source_pause(struct source *source)
{
    alSourcePause(source->source_id);
}

void source_stop(struct source *source)
{
    alSourceStop(source->source_id);
}

void source_destroy(struct source *source)
{
    alDeleteSources(1, &source->source_id);
    free(source);
}
