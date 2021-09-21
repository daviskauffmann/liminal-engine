#ifndef SCENE_HPP
#define SCENE_HPP

#include <entt/entt.hpp> // TODO: remove this include by providing a proper API, that way the client app doesnt need to have an entt dependency
#include <string>

class btDiscreteDynamicsWorld;

namespace liminal
{
    class camera;
    class skybox;
    class entity;

    class scene
    {
    public:
        liminal::camera *camera;
        liminal::skybox *skybox;
        entt::registry registry; // TODO: make private

        scene(const std::string &filename);
        ~scene();

        liminal::entity create_entity();

        void update(unsigned int current_time, float delta_time);

    private:
        btDiscreteDynamicsWorld *world;

        friend class entity;
    };
}

#endif
