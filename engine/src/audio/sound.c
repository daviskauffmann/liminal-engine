#include <engine/engine.h>

struct sound *sound_create(const char *file)
{
    struct sound *sound = malloc(sizeof(struct sound));

    if (!sound)
    {
        error("Couldn't allocate sound");

        return NULL;
    }

    alGenBuffers(1, &sound->buffer_id);

    Mix_Chunk *chunk = Mix_LoadWAV(file);

    if (!chunk)
    {
        error(Mix_GetError());

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
