#ifndef LIMINAL_CORE_AUDIO_HPP
#define LIMINAL_CORE_AUDIO_HPP

#include <SDL2/SDL_mixer.h>
#include <cstdint>

namespace liminal
{
    class audio
    {
    public:
        audio(
            int frequency = MIX_DEFAULT_FREQUENCY,
            std::uint16_t format = MIX_DEFAULT_FORMAT,
            int channels = MIX_DEFAULT_CHANNELS,
            int chunk_size = 4096);
        ~audio();
        audio(const audio &other) = delete;
        audio &operator=(const audio &other) = delete;
        audio(audio &&other) = delete;
        audio &operator=(audio &&other) = delete;
    };
}

#endif
