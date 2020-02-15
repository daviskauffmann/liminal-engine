#ifndef SOUND_HPP
#ifndef SOUND_HPP

#include <string>

#include <AL/al.h>

namespace pk
{
struct sound
{
    ALuint buffer_id;

    sound(const std::string &filename);
    ~sound();
};
} // namespace pk

#endif
