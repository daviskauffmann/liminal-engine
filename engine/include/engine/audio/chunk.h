#ifndef CHUNK_H
#define CHUNK_H

struct chunk
{
    Mix_Chunk *chunk;
};

struct chunk ENGINE_API *chunk_create(const char *file);
int ENGINE_API chunk_play(struct chunk *chunk, int channel, int loops);
void ENGINE_API chunk_destroy(struct chunk *chunk);

#endif
