#include "audio.hpp"

#include <iostream>
#include <string>

#include <AL/al.h>
#include <glm/gtc/type_ptr.hpp>
#include <SDL/SDL_mixer.h>

namespace pk
{
audio::audio()
{
    Mix_Init(0);
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);
    const SDL_version *version = Mix_Linked_Version();
    std::cout << "SDL_mixer " << std::to_string(version->major) << "." << std::to_string(version->minor) << "." << std::to_string(version->patch) << std::endl;

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

    Mix_CloseAudio();
    Mix_Quit();
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
