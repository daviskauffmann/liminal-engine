#include "sound.hpp"

#include <iostream>
#include <SDL2/SDL_mixer.h>

pk::sound::sound(const std::string &filename)
{
    alGenBuffers(1, &buffer_id);
    Mix_Chunk *chunk = Mix_LoadWAV(filename.c_str());
    if (!chunk)
    {
        std::cout << "Error: " << Mix_GetError() << std::endl;
    }
    std::cout << "Loaded: " << filename << std::endl;
    alBufferData(
        buffer_id,
        AL_FORMAT_MONO16,
        chunk->abuf,
        chunk->alen,
        44100);
    Mix_FreeChunk(chunk);
}

pk::sound::~sound()
{
    alDeleteBuffers(1, &buffer_id);
}
