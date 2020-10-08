#ifndef SCENE_HPP
#define SCENE_HPP

#include <SDL2/SDL.h>

#include "audio.hpp"
#include "renderer.hpp"

namespace pk
{
    class scene
    {
    public:
        virtual ~scene(){};

        virtual pk::scene *handle_event(SDL_Event event) = 0;
        virtual pk::scene *update(pk::audio *audio, float delta_time) = 0;
        virtual void render(pk::renderer *renderer) const = 0;
        virtual void gui() const = 0;
    };
} // namespace pk

#endif
