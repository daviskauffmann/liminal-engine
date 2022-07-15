#ifndef LIMINAL_AUDIO_SOUND_HPP
#define LIMINAL_AUDIO_SOUND_HPP

#include <AL/al.h>
#include <string>

namespace liminal
{
    class sound
    {
    public:
        sound(const std::string &filename);
        ~sound();

        ALuint get_buffer_id() const;

    private:
        ALuint buffer_id;
    };
}

#endif
