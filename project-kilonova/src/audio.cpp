#include "audio.hpp"

#include <iostream>

#include <AL/al.h>

namespace pk
{
    audio::audio()
    {
        // setup OpenAL
        this->device = alcOpenDevice(nullptr);

        if (!this->device)
        {
            std::cout << "Error: Couldn't open device" << std::endl;
        }

        this->context = alcCreateContext(device, nullptr);

        if (!this->context)
        {
            std::cout << "Error: Couldn't create context" << std::endl;
        }

        if (!alcMakeContextCurrent(context))
        {
            std::cout << "Error: Couldn't make context current" << std::endl;
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
