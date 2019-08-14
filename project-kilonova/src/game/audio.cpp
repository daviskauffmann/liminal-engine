#include <game/game.hpp>

namespace pk
{
    audio::audio()
    {
        // setup OpenAL
        this->device = alcOpenDevice(nullptr);

        if (!this->device)
        {
            throw std::exception("Couldn't open device");
        }

        this->context = alcCreateContext(device, nullptr);

        if (!this->context)
        {
            throw std::exception("Couldn't create context");
        }

        if (!alcMakeContextCurrent(context))
        {
            throw std::exception("Couldn't make context current");
        }
    }

    audio::~audio()
    {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(this->context);
        alcCloseDevice(this->device);
    }

    void audio::set_listener(vec3 position, vec3 velocity, vec3 *orientation) const
    {
        alListenerfv(AL_POSITION, (ALfloat *)position);
        alListenerfv(AL_VELOCITY, (ALfloat *)velocity);
        alListenerfv(AL_ORIENTATION, (ALfloat *)orientation);
    }
}
