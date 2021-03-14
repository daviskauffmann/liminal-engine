#include "audio.hpp"

#include <AL/al.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// TODO: error strings

liminal::audio::audio()
{
    device = alcOpenDevice(nullptr);
    if (!device)
    {
        std::cerr << "Error: Failed to open device" << std::endl;
        return;
    }

    context = alcCreateContext(device, nullptr);
    if (!context)
    {
        std::cerr << "Error: Failed to create context" << std::endl;
        return;
    }

    if (!alcMakeContextCurrent(context))
    {
        std::cerr << "Error: Failed to make context current" << std::endl;
        return;
    }
}

liminal::audio::~audio()
{
    alcMakeContextCurrent(nullptr);

    alcDestroyContext(context);

    alcCloseDevice(device);
}

void liminal::audio::set_listener(glm::vec3 position, glm::vec3 front, glm::vec3 up)
{
    alListenerfv(AL_POSITION, glm::value_ptr(position));

    glm::vec3 velocity = this->position - position;
    alListenerfv(AL_VELOCITY, glm::value_ptr(velocity));

    float orientation[] = {
        front.x, front.y, front.z,
        up.x, up.y, up.z};
    alListenerfv(AL_ORIENTATION, orientation);

    this->position = position;
}
