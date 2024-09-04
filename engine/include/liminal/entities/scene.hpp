#ifndef LIMINAL_ENTITIES_SCENE_HPP
#define LIMINAL_ENTITIES_SCENE_HPP

#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <vector>

namespace liminal
{
    class assets;
    class entity;
    class skybox;
    class world;

    class scene
    {
        friend class entity;

    public:
        std::shared_ptr<liminal::skybox> skybox;

        scene(std::shared_ptr<liminal::assets> assets);
        ~scene();
        scene(const scene &other) = delete;
        scene &operator=(const scene &other) = delete;
        scene(scene &&other) = delete;
        scene &operator=(scene &&other) = delete;

        void load(const std::string &filename);
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
        std::shared_ptr<liminal::assets> assets;
        entt::registry registry;
        std::unique_ptr<liminal::world> world;

        void on_audio_source_construct(entt::registry &registry, entt::entity id);
        void on_character_construct(entt::registry &registry, entt::entity id);
        void on_character_destroy(entt::registry &registry, entt::entity id);
        void on_physical_construct(entt::registry &registry, entt::entity id);
        void on_physical_destroy(entt::registry &registry, entt::entity id);
        void on_script_construct(entt::registry &registry, entt::entity id);
    };
}

#endif
