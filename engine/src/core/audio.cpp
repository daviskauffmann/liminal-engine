#include <liminal/core/audio.hpp>

#include <SDL2/SDL_mixer.h>
#include <stdexcept>

liminal::audio::audio()
{
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) != 0)
    {
        throw std::runtime_error(Mix_GetError());
    }
}

liminal::audio::~audio()
{
    Mix_CloseAudio();
}
