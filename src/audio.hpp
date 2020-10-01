#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <AL/alc.h>
#include <glm/vec3.hpp>

namespace pk
{
    class audio
    {
    public:
        audio();
        ~audio();

        void set_listener(glm::vec3 position, glm::vec3 front, glm::vec3 up) const;

    private:
        ALCdevice *device;
        ALCcontext *context;
        mutable glm::vec3 position;
    };
} // namespace pk

#endif
