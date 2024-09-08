#ifndef LIMINAL_COMPONENTS_AUDIO_SOURCE_HPP
#define LIMINAL_COMPONENTS_AUDIO_SOURCE_HPP

#include <glm/vec3.hpp>
#include <memory>

namespace liminal
{
    namespace audio
    {
        class source;
    }

    namespace components
    {
        struct audio_source
        {
            glm::vec3 last_position = {};
            std::shared_ptr<liminal::audio::source> source;
        };
    }
}

#endif
