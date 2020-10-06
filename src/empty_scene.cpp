#include "empty_scene.hpp"

namespace pk
{
    empty_scene::empty_scene()
    {
    }

    empty_scene::~empty_scene()
    {
    }

    scene *empty_scene::handle_event(SDL_Event event)
    {
        return this;
    }

    scene *empty_scene::update(pk::audio *audio, float delta_time)
    {
        return this;
    }

    void empty_scene::render(pk::renderer *renderer) const
    {
    }

    void empty_scene::gui() const
    {
    }

    void empty_scene::print_commands()
    {
    }

    void empty_scene::handle_command(const char *command)
    {
    }
} // namespace pk
