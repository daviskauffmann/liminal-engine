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

        virtual scene *handle_event(SDL_Event event) = 0;
        virtual scene *update(pk::audio *audio, float delta_time) = 0;
        virtual void render(pk::renderer *renderer) const = 0;
        virtual void gui() const = 0;
        virtual void print_commands() = 0;
        virtual void handle_command(const char *command) = 0;
    };
} // namespace pk

#endif
