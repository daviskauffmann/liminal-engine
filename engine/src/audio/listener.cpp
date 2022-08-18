#include <liminal/audio/listener.hpp>

#include <AL/al.h>
#include <array>
#include <glm/gtc/type_ptr.hpp>

void liminal::listener::set_position(const glm::vec3 &position)
{
    alListenerfv(AL_POSITION, glm::value_ptr(position));
}

void liminal::listener::set_velocity(const glm::vec3 &velocity)
{
    alListenerfv(AL_VELOCITY, glm::value_ptr(velocity));
}

void liminal::listener::set_orientation(const glm::vec3 &rotation, const glm::vec3 &up)
{
    const std::array<float, 6> orientation{
        rotation.x, rotation.y, rotation.z,
        up.x, up.y, up.z};
    alListenerfv(AL_ORIENTATION, orientation.data());
}
