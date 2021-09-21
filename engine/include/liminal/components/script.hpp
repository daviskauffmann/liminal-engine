#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include <entt/entt.hpp>
#include <liminal/core/scene.hpp>
#include <string>

namespace sol
{
    class state;
}

namespace liminal
{
    struct script
    {
        entt::entity entity;
        sol::state *lua;

        script(const std::string &filename, liminal::scene *scene, entt::entity entity);

        void init();
        void update(float delta_time);
    };
}

#endif
