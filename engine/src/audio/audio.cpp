#include <liminal/audio/audio.hpp>

#include <AL/al.h>
#include <glm/gtc/type_ptr.hpp>

void liminal::audio::set_listener_position(glm::vec3 position, glm::vec3 velocity, glm::vec3 front, glm::vec3 up) const
{
    alListenerfv(AL_POSITION, glm::value_ptr(position));

    alListenerfv(AL_VELOCITY, glm::value_ptr(velocity));

    float orientation[] = {
        front.x, front.y, front.z,
        up.x, up.y, up.z};
    alListenerfv(AL_ORIENTATION, orientation);
}
