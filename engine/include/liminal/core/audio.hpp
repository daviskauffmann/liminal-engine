#ifndef LIMINAL_CORE_AUDIO_HPP
#define LIMINAL_CORE_AUDIO_HPP

#include <SDL2/SDL_mixer.h>

namespace liminal
{
    class audio
    {
    public:
        audio(int frequency, Uint16 format, int channels, int chunk_size);
        ~audio();
        audio(const audio &other) = delete;
        audio &operator=(const audio &other) = delete;
        audio(audio &&other) = delete;
        audio &operator=(audio &&other) = delete;
    };
}

#endif
