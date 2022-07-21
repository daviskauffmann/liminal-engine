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
        sol::state *lua;

        script(const std::string &filename, liminal::scene *scene, entt::entity id);
        ~script();

        void init() const;
        void update(float delta_time) const;
    };
}

#endif
