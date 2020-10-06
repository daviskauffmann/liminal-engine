#ifndef EMPTY_SCENE_HPP
#define EMPTY_SCENE_HPP

#include "scene.hpp"

namespace pk
{
    class empty_scene : public scene
    {
    public:
        empty_scene();
        ~empty_scene();

        scene *handle_event(SDL_Event event) override;
        scene *update(pk::audio *audio, float delta_time) override;
        void render(pk::renderer *renderer) const override;
        void gui() const override;
        void print_commands() override;
        void handle_command(const char *command) override;
    };
} // namespace pk

#endif
