#ifndef SCENE_HPP
#define SCENE_HPP

#include <bullet/btBulletDynamicsCommon.h>
#include <entt/entt.hpp>
#include <string>

namespace liminal
{
    class scene
    {
    public:
        entt::registry registry;
        btDiscreteDynamicsWorld *world;

        scene(const std::string &filename);
        ~scene();

        void update(unsigned int current_time, float delta_time);
    };
}

#endif
