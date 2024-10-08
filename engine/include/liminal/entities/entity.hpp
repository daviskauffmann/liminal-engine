#ifndef LIMINAL_ENTITIES_ENTITY_HPP
#define LIMINAL_ENTITIES_ENTITY_HPP

#include "scene.hpp"
#include <entt/entt.hpp>

namespace liminal
{
    namespace entities
    {
        class entity
        {
        public:
            entity() = default;
            entity(const entt::entity id, liminal::entities::scene *const scene)
                : id(id),
                  scene(scene) {}

            auto operator==(const entity &other) const
            {
                return id == other.id && scene == other.scene;
            }

            auto operator!=(const entity &other) const
            {
                return !(*this == other);
            }

            operator bool() const
            {
                return id != entt::null;
            }

            operator uint64_t() const
            {
                return (uint64_t)id;
            }

            operator entt::entity() const
            {
                return id;
            }

            template <typename... Components>
            auto has_components() const
            {
                return scene->registry.all_of<Components...>(id);
            }

            template <typename Component>
            auto &get_component() const
            {
                return scene->registry.get<Component>(id);
            }

            template <typename Component, typename... Args>
            auto &add_component(Args &&...args)
            {
                return scene->registry.emplace<Component>(id, std::forward<Args>(args)...);
            }

            template <typename Component>
            auto remove_component()
            {
                scene->registry.remove<Component>(id);
            }

        private:
            entt::entity id = entt::null;
            liminal::entities::scene *scene = nullptr;
        };
    }
}

#endif
