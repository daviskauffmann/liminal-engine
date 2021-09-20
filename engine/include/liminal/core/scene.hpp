#ifndef SCENE_HPP
#define SCENE_HPP

#include <entt/entt.hpp> // TODO: might be able to remove this include if a proper API is provided by the class; that way the client app doesnt need to have an entt dependency
#include <string>

namespace liminal
{
    class scene
    {
    public:
        entt::registry registry;

        scene(const std::string &filename);
        ~scene();

        void update(unsigned int current_time, float delta_time);
    };
}

#endif
