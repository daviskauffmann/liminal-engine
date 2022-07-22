#include <liminal/components/audio_listener.hpp>

#include <AL/al.h>
#include <glm/gtc/type_ptr.hpp>

liminal::audio_listener::audio_listener()
{
    alListenerfv(AL_POSITION, glm::value_ptr(last_position));
}

void liminal::audio_listener::set_position(
    const glm::vec3 &position,
    const glm::vec3 &rotation)
{
    alListenerfv(AL_POSITION, glm::value_ptr(position));

    glm::vec3 velocity = last_position - position;
    alListenerfv(AL_VELOCITY, glm::value_ptr(velocity));

    const float orientation[] = {
        rotation.x, rotation.y, rotation.z,
        0, 1, 0};
    alListenerfv(AL_ORIENTATION, orientation);

    last_position = position;
}
