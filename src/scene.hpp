#ifndef SCENE_HPP
#define SCENE_HPP

#include <bullet/btBulletDynamicsCommon.h>
#include <entt/entt.hpp>
#include <string>

namespace liminal
{
    class engine;

    class scene
    {
    public:
        entt::registry registry;
        btDiscreteDynamicsWorld *world;

        scene(const std::string &filename, liminal::engine *engine);
        ~scene();
    };
}

#endif
