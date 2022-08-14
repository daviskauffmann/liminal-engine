#ifndef LIMINAL_CORE_AUDIO_HPP
#define LIMINAL_CORE_AUDIO_HPP

namespace liminal
{
    class audio
    {
    public:
        audio();
        audio(audio &&other) = delete;
        audio(const audio &other) = delete;
        audio &operator=(audio &&other) = delete;
        audio &operator=(const audio &other) = delete;
        ~audio();
    };
}

#endif
