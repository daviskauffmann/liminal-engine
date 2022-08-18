#ifndef LIMINAL_ENTITIES_SCENE_HPP
#define LIMINAL_ENTITIES_SCENE_HPP

#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <vector>

class btDiscreteDynamicsWorld;

namespace liminal
{
    class assets;
    class skybox;
    class source;
    class entity;

    class scene
    {
        friend class entity;

    public:
        std::shared_ptr<liminal::skybox> skybox;

        scene();
        scene(const liminal::scene &other);
        ~scene();

        void load(const std::string &filename, std::shared_ptr<liminal::assets> assets);
        void save(const std::string &filename);

        liminal::entity create_entity();
        liminal::entity get_entity(entt::entity id);
        void delete_entity(liminal::entity entity);

        void start();
        void update(std::uint64_t current_time, float delta_time);
        void stop();

        void reload_scripts();

        // TODO: is there any way to make this const?
        template <typename... Components>
        auto get_entities_with()
        {
            return registry.view<Components...>();
        }

    private:
        entt::registry registry;
        std::vector<std::shared_ptr<liminal::source>> sources;
        std::unique_ptr<btDiscreteDynamicsWorld> world;

        void on_audio_source_construct(entt::registry &registry, entt::entity id);
        void on_audio_source_destroy(entt::registry &registry, entt::entity id);
        void on_physical_construct(entt::registry &registry, entt::entity id);
        void on_physical_destroy(entt::registry &registry, entt::entity id);
    };
}

#endif
