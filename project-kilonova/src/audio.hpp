#ifndef GAME_AUDIO_HPP
#define GAME_AUDIO_HPP

#include <AL/alc.h>
#include <cglm/cglm.h>

namespace pk
{
    class audio
    {
    public:
        audio();
        ~audio();

        void set_listener(vec3 position, vec3 velocity, vec3 *orientation) const;
    private:
        ALCdevice *device;
        ALCcontext *context;
    };
}

#endif
