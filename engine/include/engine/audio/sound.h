#ifndef SOUND_H
#define SOUND_H

struct sound
{
    ALuint buffer_id;
};

struct sound ENGINE_API *sound_create(const void *data, int size);
void ENGINE_API sound_destroy(struct sound *sound);

#endif
