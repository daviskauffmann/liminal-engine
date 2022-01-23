#ifndef LIMINAL_COMPONENTS_AUDIO_SOURCE_HPP
#define LIMINAL_COMPONENTS_AUDIO_SOURCE_HPP

#include <liminal/audio/source.hpp>

namespace liminal
{
    struct audio_source
    {
        liminal::source *source;

        audio_source();
        ~audio_source();
    };
}

#endif
