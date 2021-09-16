#ifndef SCENE_HPP
#define SCENE_HPP

#include <bullet/btBulletDynamicsCommon.h>
#include <entt/entt.hpp>
#include <string>

#include "../graphics/camera.hpp"
#include "../graphics/skybox.hpp"

namespace liminal
{
    class scene
    {
    public:
        entt::registry registry;
        btDiscreteDynamicsWorld *world;
        liminal::camera *camera;
        liminal::skybox *skybox;

        scene(const std::string &filename);
        ~scene();
    };
}

#endif
