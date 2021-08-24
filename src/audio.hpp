#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <AL/alc.h>
#include <glm/vec3.hpp>

namespace liminal
{
    class audio
    {
    public:
        audio();
        ~audio();

        void set_listener(glm::vec3 position, glm::vec3 front, glm::vec3 up);

    private:
        ALCdevice *device;
        ALCcontext *context;
        glm::vec3 position;
    };
}

#endif
