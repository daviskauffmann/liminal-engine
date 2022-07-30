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

        ALuint get_buffer_id() const;

    private:
        ALuint buffer_id = 0;
    };
}

#endif
