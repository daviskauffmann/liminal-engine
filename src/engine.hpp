#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "audio.hpp"
#include "platform.hpp"
#include "renderer.hpp"

namespace liminal
{
    class engine
    {
    public:
        liminal::platform *platform;
        liminal::renderer *renderer;
        liminal::audio *audio;

        engine(const std::string &window_title, int window_width, int window_height, float render_scale);
        ~engine();
    };
}

#endif
