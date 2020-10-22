#include "sound.hpp"

#include <SDL2/SDL_mixer.h>
#include <spdlog/spdlog.h>

pk::sound::sound(const std::string &filename)
{
    Mix_Chunk *chunk = Mix_LoadWAV(filename.c_str());
    if (!chunk)
    {
        spdlog::error("Failed to load sound: {}", Mix_GetError());
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

pk::sound::~sound()
{
    alDeleteBuffers(1, &buffer_id);
}
