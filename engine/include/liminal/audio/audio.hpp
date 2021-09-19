#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <glm/vec3.hpp>

namespace liminal
{
    class audio
    {
    public:
        void set_listener_position(glm::vec3 position, glm::vec3 velocity, glm::vec3 front, glm::vec3 up) const;
    };
}

#endif
