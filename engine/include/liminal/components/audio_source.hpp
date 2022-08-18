#ifndef LIMINAL_COMPONENTS_AUDIO_SOURCE_HPP
#define LIMINAL_COMPONENTS_AUDIO_SOURCE_HPP

#include <glm/vec3.hpp>
#include <memory>

namespace liminal
{
    class source;

    struct audio_source
    {
        std::shared_ptr<liminal::source> source;
        glm::vec3 last_position = {};
    };
}

#endif
