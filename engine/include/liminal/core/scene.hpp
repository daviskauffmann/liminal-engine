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

        // TODO: work on this more, basically just want a good way to request that the renderer not draw directly to the default framebuffer
        // but put it in a texture instead so the application can use it
        // this code probably shouldn't be here, but just testing
        bool draw_to_texture = false;
        void *texture_id = 0;

        scene();
        ~scene();

        void load(const std::string &filename);
        void save(const std::string &filename);

        liminal::entity create_entity();
        liminal::entity get_entity(entt::entity id);

        void update(unsigned int current_time, float delta_time);

    private:
        btDiscreteDynamicsWorld *world;
        std::unordered_map<entt::entity, liminal::entity> entities;

        friend class entity;
    };
}

#endif
