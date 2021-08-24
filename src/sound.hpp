#ifndef SOUND_HPP
#define SOUND_HPP

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
