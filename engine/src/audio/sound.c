#include <engine/engine.h>

struct sound *sound_create(const void *data, int size)
{
    struct sound *sound = malloc(sizeof(struct sound));

    if (!sound)
    {
        printf("Error: Couldn't allocate sound\n");

        return NULL;
    }

    alGenBuffers(1, &sound->buffer_id);

    alBufferData(
        sound->buffer_id,
        AL_FORMAT_MONO16,
        data,
        size,
        44100);

    return sound;
}

void sound_destroy(struct sound *sound)
{
    alDeleteBuffers(1, &sound->buffer_id);
    free(sound);
}
