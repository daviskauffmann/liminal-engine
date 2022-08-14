#include <liminal/core/sdl_image.hpp>

#include <SDL2/SDL_image.h>
#include <stdexcept>

liminal::sdl_image::sdl_image()
{
    constexpr int img_flags = IMG_INIT_PNG;
    if (IMG_Init(img_flags) != img_flags)
    {
        throw std::runtime_error(IMG_GetError());
    }
}

liminal::sdl_image::~sdl_image()
{
    IMG_Quit();
}
