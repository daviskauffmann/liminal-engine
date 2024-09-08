#ifndef LIMINAL_COMPONENTS_AUDIO_LISTENER_HPP
#define LIMINAL_COMPONENTS_AUDIO_LISTENER_HPP

#include <glm/vec3.hpp>

namespace liminal
{
    namespace components
    {
        struct audio_listener
        {
            glm::vec3 last_position = {};
        };
    }
}

#endif
