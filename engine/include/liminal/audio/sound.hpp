#ifndef LIMINAL_AUDIO_SOUND_HPP
#define LIMINAL_AUDIO_SOUND_HPP

#include <AL/al.h>

namespace liminal
{
    class sound
    {
    public:
        sound(const char *filename);
        ~sound();
        sound(const sound &other) = delete;
        sound &operator=(const sound &other) = delete;
        sound(sound &&other) = delete;
        sound &operator=(sound &&other) = delete;

        ALuint get_buffer_id() const;

    private:
        ALuint buffer_id;
    };
}

#endif
