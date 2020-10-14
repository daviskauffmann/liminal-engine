#ifndef GAME_SCENE_HPP
#define GAME_SCENE_HPP

#include <bullet/btBulletDynamicsCommon.h>

#include "audio.hpp"
#include "camera.hpp"
#include "directional_light.hpp"
#include "model.hpp"
#include "object.hpp"
#include "point_light.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "skybox.hpp"
#include "sound.hpp"
#include "source.hpp"
#include "spot_light.hpp"
#include "sprite.hpp"
#include "terrain.hpp"
#include "water.hpp"

namespace pk
{
    class game_scene : public pk::scene
    {
    public:
        game_scene();
        ~game_scene();

        pk::scene *handle_event(SDL_Event event) override;
        pk::scene *update(pk::audio *audio, float delta_time) override;
        void render(pk::renderer *renderer) const override;
        void gui() const override;

    private:
        btDefaultCollisionConfiguration *collision_configuration;
        btCollisionDispatcher *dispatcher;
        btBroadphaseInterface *overlapping_pair_cache;
        btSequentialImpulseConstraintSolver *solver;
        btDiscreteDynamicsWorld *world;

        pk::camera *camera;

        pk::skybox *skybox;

        pk::model *cube_model;
        pk::object *cube;

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

        bool edit_mode;
        bool lock_cursor;
        bool torch_on;
        bool torch_follow;
    };
} // namespace pk

#endif
