#ifndef LIMINAL_CORE_SCENE_HPP
#define LIMINAL_CORE_SCENE_HPP

#include <entt/entt.hpp> // TODO: remove this include by providing a proper API, that way the client app doesnt need to have an entt dependency
#include <imgui.h>
#include <string>
#include <unordered_map>

class btDiscreteDynamicsWorld;

namespace liminal
{
    struct camera;
    struct skybox;
    class entity;

    class scene
    {
    public:
        liminal::camera *camera;
        liminal::skybox *skybox;
        entt::registry registry; // TODO: make private

        scene();
        ~scene();

        void load(const std::string &filename);
        void save(const std::string &filename);

        liminal::entity create_entity();
        liminal::entity get_entity(entt::entity id);

        void update(unsigned int current_time, float delta_time);

        void reload_scripts();

    private:
        btDiscreteDynamicsWorld *world;
        std::unordered_map<entt::entity, liminal::entity> entities;

        friend class entity;
    };
}

#endif
