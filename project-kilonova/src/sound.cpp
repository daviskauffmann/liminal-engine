#include "sound.hpp"

#include <iostream>

#include <SDL/SDL_mixer.h>

namespace pk
{
sound::sound(const std::string &filename)
{
    alGenBuffers(1, &this->buffer_id);
    Mix_Chunk *chunk = Mix_LoadWAV(filename.c_str());
    if (!chunk)
    {
        std::cout << "Error: Couldn't load chunk" << std::endl;
    }
    alBufferData(
        this->buffer_id,
        AL_FORMAT_MONO16,
        chunk->abuf,
        chunk->alen,
        44100);
    Mix_FreeChunk(chunk);
}

sound::~sound()
{
    alDeleteBuffers(1, &this->buffer_id);
}
} // namespace pk