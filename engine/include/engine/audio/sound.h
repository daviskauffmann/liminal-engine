#ifndef SOUND_H
#define SOUND_H

struct sound
{
    ALuint buffer_id;
};

struct sound ENGINE_API *sound_create(const char *file);
void ENGINE_API sound_destroy(struct sound *sound);

#endif
