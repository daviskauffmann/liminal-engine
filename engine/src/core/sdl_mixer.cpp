#include <liminal/core/sdl_mixer.hpp>

#include <SDL2/SDL_mixer.h>
#include <stdexcept>

liminal::sdl_mixer::sdl_mixer()
{
    constexpr int flags = 0;
    if (Mix_Init(flags) != flags)
    {
        throw std::runtime_error(Mix_GetError());
    }

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) != 0)
    {
        Mix_Quit();

        throw std::runtime_error(Mix_GetError());
    }
}

liminal::sdl_mixer::~sdl_mixer()
{
    Mix_CloseAudio();
    Mix_Quit();
}
