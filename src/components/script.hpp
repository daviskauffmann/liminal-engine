#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include <entt/entt.hpp>
#include <string>
#include <sol/sol.hpp>

#include "../scene.hpp"

namespace liminal
{
    struct script
    {
        script(const std::string &filename, liminal::scene *scene, entt::entity entity);
        ~script();

        void init();
        void update(float delta_time);

    private:
        entt::entity entity;
        sol::state *lua;
    };
}

#endif
