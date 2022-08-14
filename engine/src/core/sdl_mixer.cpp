#include <liminal/core/sdl_mixer.hpp>

#include <SDL2/SDL_mixer.h>
#include <stdexcept>

liminal::sdl_mixer::sdl_mixer()
{
    constexpr int mix_flags = 0;
    if (Mix_Init(mix_flags) != mix_flags)
    {
        throw std::runtime_error(Mix_GetError());
    }
}

liminal::sdl_mixer::~sdl_mixer()
{
    Mix_Quit();
}
