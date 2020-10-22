#include "audio.hpp"

#include <AL/al.h>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

// TODO: error strings

pk::audio::audio()
{
    device = alcOpenDevice(nullptr);
    if (!device)
    {
        spdlog::error("Failed to open device");
        return;
    }

    context = alcCreateContext(device, nullptr);
    if (!context)
    {
        spdlog::error("Failed to create context");
        return;
    }

    if (!alcMakeContextCurrent(context))
    {
        spdlog::error("Failed to make context current");
        return;
    }
}

pk::audio::~audio()
{
    alcMakeContextCurrent(nullptr);

    alcDestroyContext(context);

    alcCloseDevice(device);
}

void pk::audio::set_listener(glm::vec3 position, glm::vec3 front, glm::vec3 up) const
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
