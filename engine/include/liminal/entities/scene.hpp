#ifndef LIMINAL_ENTITIES_SCENE_HPP
#define LIMINAL_ENTITIES_SCENE_HPP

#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <vector>

namespace liminal
{
    namespace core
    {
        class assets;
    }

    namespace graphics
    {
        class skybox;
    }

    namespace physics
    {
        class world;
    }

    namespace entities
    {
        class entity;

        class scene
        {
            friend class liminal::entities::entity;

        public:
            std::shared_ptr<liminal::graphics::skybox> skybox;

            scene(std::shared_ptr<liminal::core::assets> assets);
            ~scene();
            scene(const scene &other) = delete;
            scene &operator=(const scene &other) = delete;
            scene(scene &&other) = delete;
            scene &operator=(scene &&other) = delete;

            void load(const std::string &filename);
            void save(const std::string &filename);

            liminal::entities::entity create_entity();
            liminal::entities::entity get_entity(entt::entity id);
            void delete_entity(liminal::entities::entity entity);

            void start();
            void update(std::uint64_t current_time, float delta_time);
            void stop();

            void reload_scripts();

            template <typename... Components>
            auto get_entities_with()
            {
                return registry.view<Components...>();
            }

            liminal::entities::scene *copy();

        private:
            std::shared_ptr<liminal::core::assets> assets;
            entt::registry registry;
            std::unique_ptr<liminal::physics::world> world;

            void on_audio_source_construct(entt::registry &registry, entt::entity id);
            void on_character_construct(entt::registry &registry, entt::entity id);
            void on_character_destroy(entt::registry &registry, entt::entity id);
            void on_physical_construct(entt::registry &registry, entt::entity id);
            void on_physical_destroy(entt::registry &registry, entt::entity id);
            void on_script_construct(entt::registry &registry, entt::entity id);
        };
    }
}

#endif
