#ifndef LIMINAL_CORE_SDL_IMAGE_HPP
#define LIMINAL_CORE_SDL_IMAGE_HPP

namespace liminal
{
    class sdl_image
    {
    public:
        sdl_image();
        sdl_image(sdl_image &&other) = delete;
        sdl_image(const sdl_image &other) = delete;
        sdl_image &operator=(sdl_image &&other) = delete;
        sdl_image &operator=(const sdl_image &other) = delete;
        ~sdl_image();
    };
}

#endif
