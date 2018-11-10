#include <engine/engine.h>

struct chunk *chunk_create(const char *file)
{
    struct chunk *chunk = malloc(sizeof(struct chunk));

    if (!chunk)
    {
        error("Couldn't allocate chunk");

        return NULL;
    }

    chunk->chunk = Mix_LoadWAV(file);

    if (!chunk->chunk)
    {
        error(Mix_GetError());

        return NULL;
    }

    return chunk;
}

int chunk_play(struct chunk *chunk, int channel, int loops)
{
    return Mix_PlayChannel(channel, chunk->chunk, loops);
}

void chunk_destroy(struct chunk *chunk)
{
    Mix_FreeChunk(chunk->chunk);
    free(chunk);
}
