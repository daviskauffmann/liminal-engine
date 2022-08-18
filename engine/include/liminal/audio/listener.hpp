#ifndef LIMINAL_AUDIO_LISTENER_HPP
#define LIMINAL_AUDIO_LISTENER_HPP

#include <glm/vec3.hpp>

namespace liminal
{
    class listener
    {
    public:
        static void set_position(const glm::vec3 &position);
        static void set_velocity(const glm::vec3 &velocity);
        static void set_orientation(const glm::vec3 &rotation, const glm::vec3 &up);
    };
}

#endif
