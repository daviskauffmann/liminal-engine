#ifndef SCENE_HPP
#define SCENE_HPP

#include <SDL2/SDL.h>

#include "audio.hpp"
#include "camera.hpp"
#include "directional_light.hpp"
#include "model.hpp"
#include "object.hpp"
#include "point_light.hpp"
#include "renderer.hpp"
#include "skybox.hpp"
#include "sound.hpp"
#include "source.hpp"
#include "spot_light.hpp"
#include "sprite.hpp"
#include "terrain.hpp"
#include "water.hpp"

namespace pk
{
    class scene
    {
    public:
        scene();
        virtual ~scene();

        virtual scene *handle_event(SDL_Event event) = 0;
        virtual scene *update(pk::audio *audio, float delta_time) = 0;
        virtual void render(pk::renderer *renderer) const = 0;
        virtual void gui() const = 0;
    };

    class game_scene : public scene
    {
    public:
        game_scene();
        ~game_scene();

        scene *handle_event(SDL_Event event) override;
        scene *update(pk::audio *audio, float delta_time) override;
        void render(pk::renderer *renderer) const override;
        void gui() const override;

    private:
        pk::camera *camera;

        pk::skybox *skybox;

        pk::model *backpack_model;
        pk::object *backpack;

        pk::directional_light *sun;

        pk::point_light *red_light;
        pk::point_light *yellow_light;
        pk::point_light *green_light;
        pk::point_light *blue_light;

        pk::spot_light *torch;

        pk::water *water;

        pk::terrain *terrain;

        pk::source *ambient_source;
        pk::source *bounce_source;
        pk::source *shoot_source;

        pk::sound *ambient_sound;
        pk::sound *bounce_sound;
        pk::sound *shoot_sound;

        bool torch_on;
        bool torch_follow;
        bool edit_mode;
    };
} // namespace pk

#endif
