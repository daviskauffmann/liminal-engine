#include <liminal/audio/sound.hpp>

#include <SDL2/SDL_mixer.h>
#include <iostream>

liminal::sound::sound(const char *const filename)
{
    Mix_Chunk *chunk = Mix_LoadWAV(filename);
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

ALuint liminal::sound::get_buffer_id() const
{
    return buffer_id;
}
