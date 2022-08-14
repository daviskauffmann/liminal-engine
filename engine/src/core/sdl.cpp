#include <liminal/core/sdl.hpp>

#include <SDL2/SDL.h>
#include <stdexcept>

liminal::sdl::sdl()
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0)
    {
        throw std::runtime_error(SDL_GetError());
    }
}

liminal::sdl::~sdl()
{
    SDL_Quit();
}

std::uint64_t liminal::sdl::get_ticks() const
{
    return SDL_GetTicks64();
}

const std::uint8_t *liminal::sdl::get_keys() const
{
    return SDL_GetKeyboardState(nullptr);
}

std::uint32_t liminal::sdl::get_mouse_state(int *const x, int *const y) const
{
    return SDL_GetMouseState(x, y);
}

bool liminal::sdl::get_relative_mouse_mode() const
{
    return SDL_GetRelativeMouseMode();
}

void liminal::sdl::set_relative_mouse_mode(const bool enabled) const
{
    if (SDL_SetRelativeMouseMode(static_cast<SDL_bool>(enabled)) != 0)
    {
        throw std::runtime_error(SDL_GetError());
    }
}

bool liminal::sdl::poll_event(SDL_Event &event)
{
    return SDL_PollEvent(&event);
}
