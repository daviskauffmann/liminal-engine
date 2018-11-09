#ifndef CHUNK_H
#define CHUNK_H

struct chunk
{
    Mix_Chunk chunk;
};

struct chunk ENGINE_API *chunk_create(const char *file);
void ENGINE_API chunk_destroy(struct chunk *chunk);

#endif
