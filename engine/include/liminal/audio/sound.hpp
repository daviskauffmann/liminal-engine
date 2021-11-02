#ifndef LIMINAL_AUDIO_SOUND_HPP
#define LIMINAL_AUDIO_SOUND_HPP

#include <string>

namespace liminal
{
    struct sound
    {
        unsigned int buffer_id;

        sound(const std::string &filename);
        ~sound();
    };
}

#endif
