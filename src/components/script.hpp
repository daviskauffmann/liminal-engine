#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include <string>
#include <sol/sol.hpp>

namespace liminal
{
    struct script
    {
        script(sol::state *lua);
        ~script();

        void update(float delta_time);

    private:
        sol::state *lua;
        sol::function lua_init;
        sol::function lua_update;
    };
}

#endif
