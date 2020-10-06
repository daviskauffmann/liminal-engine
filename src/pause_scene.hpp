#ifndef PAUSE_SCENE_HPP
#define PAUSE_SCENE_HPP

#include "scene.hpp"

namespace pk
{
    class pause_scene : public scene
    {
    public:
        pause_scene(pk::scene *paused_scene);
        ~pause_scene();

        scene *handle_event(SDL_Event event) override;
        scene *update(pk::audio *audio, float delta_time) override;
        void render(pk::renderer *renderer) const override;
        void gui() const override;
        void print_commands() override;
        void handle_command(const char *command) override;

    private:
        pk::scene *paused_scene;
    };
} // namespace pk

#endif
