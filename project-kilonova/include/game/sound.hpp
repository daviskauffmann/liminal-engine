#ifndef GAME_SOUND_HPP
#define GAME_SOUND_HPP

struct Sound
{
    ALuint bufferId;

    Sound(const std::string &filename);
    ~Sound();
};

#endif
