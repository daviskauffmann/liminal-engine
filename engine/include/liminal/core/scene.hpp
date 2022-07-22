#ifndef LIMINAL_CORE_SCENE_HPP
#define LIMINAL_CORE_SCENE_HPP

#include <entt/entt.hpp>
#include <imgui.h>
#include <string>
#include <unordered_map>

class btDiscreteDynamicsWorld;

namespace liminal
{
    struct skybox;
    class entity;

    class scene
    {
        friend class entity;
        
    public:
        liminal::skybox *skybox;

        scene();
        scene(const liminal::scene &other);
        ~scene();

        void load(const std::string &filename);
        void save(const std::string &filename);

        liminal::entity create_entity();
        liminal::entity get_entity(entt::entity id);
        void delete_entity(liminal::entity entity);

        void start();
        void update(unsigned int current_time, float delta_time);
        void stop();

        void reload_scripts();

        template <typename... Components>
        auto get_entities_with()
        {
            return registry.view<Components...>();
        }

    private:
        entt::registry registry;
        btDiscreteDynamicsWorld *world;
    };
}

#endif
