#include <liminal/core/sdl_image.hpp>

#include <SDL2/SDL_image.h>
#include <stdexcept>

liminal::core::sdl_image::sdl_image()
{
    constexpr int flags = IMG_INIT_PNG;
    if (IMG_Init(flags) != flags)
    {
        throw std::runtime_error(IMG_GetError());
    }
}

liminal::core::sdl_image::~sdl_image()
{
    IMG_Quit();
}
