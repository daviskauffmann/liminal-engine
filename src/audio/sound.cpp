#include "sound.hpp"

#include <iostream>
#include <SDL2/SDL_mixer.h>

liminal::sound::sound(const std::string &filename)
{
    Mix_Chunk *chunk = Mix_LoadWAV(filename.c_str());
    if (!chunk)
    {
        std::cerr << "Error: Failed to load sound: " << Mix_GetError() << std::endl;
        return;
    }

    alGenBuffers(1, &buffer_id);
    alBufferData(
        buffer_id,
        AL_FORMAT_MONO16,
        chunk->abuf,
        chunk->alen,
        44100);

    Mix_FreeChunk(chunk);
}

liminal::sound::~sound()
{
    alDeleteBuffers(1, &buffer_id);
}
