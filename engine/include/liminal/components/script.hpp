#ifndef LIMINAL_COMPONENTS_SCRIPT_HPP
#define LIMINAL_COMPONENTS_SCRIPT_HPP

#include <entt/entt.hpp>
#include <string>

namespace sol
{
    class state;
}

namespace liminal
{
    class scene;
    class entity;

    class script
    {
    public:
        // TODO: support multiple script instances in a single script component?
        // because we can't have multiple of the same component on an entity
        sol::state *lua;

        script(const std::string &filename, liminal::scene *scene, entt::entity id);
        ~script();

        void init() const;
        void update(float delta_time) const;
    };
}

#endif
