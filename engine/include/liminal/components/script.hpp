#ifndef LIMINAL_COMPONENTS_SCRIPT_HPP
#define LIMINAL_COMPONENTS_SCRIPT_HPP

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
        liminal::scene *scene;
        liminal::entity *entity;
        sol::state *lua;

        script(const std::string &filename, liminal::scene *scene, liminal::entity *entity);

        void init();
        void update(float delta_time);
    };
}

#endif
