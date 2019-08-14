#include <game/game.hpp>

namespace pk
{
    sound::sound(const std::string &filename)
    {
        alGenBuffers(1, &this->buffer_id);

        Mix_Chunk *chunk = Mix_LoadWAV(filename.c_str());

        if (!chunk)
        {
            throw std::exception("Couldn't load chunk");
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
}
