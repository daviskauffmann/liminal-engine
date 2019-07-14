#include <engine/engine.h>

struct sound *sound_create(const char *filename)
{
    struct sound *sound = malloc(sizeof(struct sound));

    if (!sound)
    {
        printf("Error: Couldn't allocate sound\n");

        return NULL;
    }

    alGenBuffers(1, &sound->buffer_id);

    Mix_Chunk *chunk = Mix_LoadWAV(filename);

    if (!chunk)
    {
        printf("Error: Couldn't load chunk\n");

        return NULL;
    }

    alBufferData(
        sound->buffer_id,
        AL_FORMAT_MONO16,
        chunk->abuf,
        chunk->alen,
        44100);

    Mix_FreeChunk(chunk);

    return sound;
}

void sound_destroy(struct sound *sound)
{
    alDeleteBuffers(1, &sound->buffer_id);
    free(sound);
}
