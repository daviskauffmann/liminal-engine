#include <liminal/audio/sound.hpp>

#include <SDL2/SDL_mixer.h>
#include <stdexcept>

liminal::audio::sound::sound(const char *const filename)
{
    Mix_Chunk *chunk = Mix_LoadWAV(filename);
    if (!chunk)
    {
        throw std::runtime_error(Mix_GetError());
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

liminal::audio::sound::~sound()
{
    alDeleteBuffers(1, &buffer_id);
}

ALuint liminal::audio::sound::get_buffer_id() const
{
    return buffer_id;
}
