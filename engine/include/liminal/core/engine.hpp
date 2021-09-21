#ifndef ENGINE_HPP
#define ENGINE_HPP

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

        ~engine();

        void run(int argc, char *argv[]);

    private:
        engine();
    };
}

#endif
