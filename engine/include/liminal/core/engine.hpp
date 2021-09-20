#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <liminal/audio/audio.hpp>
#include <liminal/core/physics.hpp>
#include <liminal/core/platform.hpp>
#include <liminal/graphics/renderer.hpp>

namespace liminal
{
    class engine
    {
    public:
        static engine &get_instance();

        liminal::platform *platform;
        liminal::renderer *renderer;
        liminal::audio *audio;
        liminal::physics *physics;

        ~engine();

        void run(int argc, char *argv[]);

    private:
        engine();
    };
}

#endif
