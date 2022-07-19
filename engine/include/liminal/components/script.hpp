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

    struct script
    {
        sol::state *lua;

        script(const std::string &filename, liminal::scene *scene, entt::entity id);
        ~script();

        void init();
        void update(float delta_time);
    };
}

#endif
