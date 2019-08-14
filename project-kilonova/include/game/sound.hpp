#ifndef GAME_SOUND_HPP
#define GAME_SOUND_HPP

namespace pk
{
    struct sound
    {
        ALuint buffer_id;

        sound(const std::string &filename);
        ~sound();
    };
}

#endif
