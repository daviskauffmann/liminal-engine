#ifndef GAME_AUDIO_HPP
#define GAME_AUDIO_HPP

class Audio
{
public:
    Audio();
    ~Audio();

    void setListener(vec3 position, vec3 velocity, vec3 *orientation) const;
private:
    ALCdevice *device;
    ALCcontext *context;
};

#endif
