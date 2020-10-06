#include "empty_scene.hpp"

pk::empty_scene::empty_scene()
{
}

pk::empty_scene::~empty_scene()
{
}

pk::scene *pk::empty_scene::handle_event(SDL_Event event)
{
    return this;
}

pk::scene *pk::empty_scene::update(pk::audio *audio, float delta_time)
{
    return this;
}

void pk::empty_scene::render(pk::renderer *renderer) const
{
}

void pk::empty_scene::gui() const
{
}

void pk::empty_scene::print_commands()
{
}

void pk::empty_scene::handle_command(const char *command)
{
}
