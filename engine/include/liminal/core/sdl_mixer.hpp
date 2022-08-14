#ifndef LIMINAL_CORE_SDL_MIXER_HPP
#define LIMINAL_CORE_SDL_MIXER_HPP

namespace liminal
{
    class sdl_mixer
    {
    public:
        sdl_mixer();
        sdl_mixer(sdl_mixer &&other) = delete;
        sdl_mixer(const sdl_mixer &other) = delete;
        sdl_mixer &operator=(sdl_mixer &&other) = delete;
        sdl_mixer &operator=(const sdl_mixer &other) = delete;
        ~sdl_mixer();
    };
}

#endif
