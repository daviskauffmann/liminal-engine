#ifndef LIMINAL_CORE_ENTITY_HPP
#define LIMINAL_CORE_ENTITY_HPP

#include <entt/entt.hpp>
#include <liminal/core/scene.hpp>

namespace liminal
{
    class entity
    {
    public:
        entity() = default;
        entity(entt::entity id, liminal::scene *scene)
            : id(id),
              scene(scene) {}
        entity(const entity &other) = default;

        bool operator==(const entity &other)
        {
            return id == other.id && scene == other.scene;
        }

        bool operator!=(const entity &other)
        {
            return !(*this == other);
        }

        operator bool() const
        {
            return id != entt::null;
        }

        operator entt::entity() const
        {
            return id;
        }

        template <typename... Components>
        bool has_components()
        {
            return scene->registry.all_of<Components...>(id);
        }

        template <typename Component>
        Component &get_component()
        {
            return scene->registry.get<Component>(id);
        }

        template <typename Component, typename... Args>
        Component &add_component(Args &&...args)
        {
            return scene->registry.emplace<Component>(id, std::forward<Args>(args)...);
        }

        template <typename Component>
        void remove_component()
        {
            scene->registry.remove<Component>(id);
        }

    private:
        entt::entity id = entt::null;
        liminal::scene *scene = nullptr;
    };
}

#endif
