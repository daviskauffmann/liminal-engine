#ifndef LIMINAL_COMPONENTS_AUDIO_LISTENER_HPP
#define LIMINAL_COMPONENTS_AUDIO_LISTENER_HPP

#include <glm/vec3.hpp>

namespace liminal
{
    class audio_listener
    {
    public:
        audio_listener();

        void set_position(const glm::vec3 &position, const glm::vec3 &rotation);

    private:
        glm::vec3 last_position = {};
    };
}

#endif
