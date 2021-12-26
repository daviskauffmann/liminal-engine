#ifndef LIMINAL_AUDIO_SOUND_HPP
#define LIMINAL_AUDIO_SOUND_HPP

#include <AL/al.h>
#include <string>

namespace liminal
{
    struct sound
    {
        ALuint buffer_id;

        sound(const std::string &filename);
        ~sound();
    };
}

#endif
