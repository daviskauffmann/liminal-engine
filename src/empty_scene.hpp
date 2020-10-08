#ifndef EMPTY_SCENE_HPP
#define EMPTY_SCENE_HPP

#include "scene.hpp"

namespace pk
{
    class empty_scene : public pk::scene
    {
    public:
        empty_scene();
        ~empty_scene();

        pk::scene *handle_event(SDL_Event event) override;
        pk::scene *update(pk::audio *audio, float delta_time) override;
        void render(pk::renderer *renderer) const override;
        void gui() const override;
    };
} // namespace pk

#endif
