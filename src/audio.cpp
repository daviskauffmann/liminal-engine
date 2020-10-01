#include "audio.hpp"

#include <AL/al.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace pk
{
    audio::audio()
    {
        device = alcOpenDevice(nullptr);
        if (!device)
        {
            std::cout << "Error: Couldn't open device" << std::endl;
        }
        context = alcCreateContext(device, nullptr);
        if (!context)
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
        alcDestroyContext(context);
        alcCloseDevice(device);
    }

    void audio::set_listener(glm::vec3 position, glm::vec3 front, glm::vec3 up) const
    {
        glm::vec3 velocity = this->position - position;
        float orientation[] = {
            front.x, front.y, front.z,
            up.x, up.y, up.z};
        alListenerfv(AL_POSITION, glm::value_ptr(position));
        alListenerfv(AL_VELOCITY, glm::value_ptr(velocity));
        alListenerfv(AL_ORIENTATION, orientation);
        this->position = position;
    }
} // namespace pk
