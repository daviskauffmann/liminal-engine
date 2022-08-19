#ifndef LIMINAL_COMPONENTS_SCRIPT_HPP
#define LIMINAL_COMPONENTS_SCRIPT_HPP

#include <memory>
#include <string>

namespace liminal
{
    class lua_state;

    struct script
    {
        std::string filename;
        std::shared_ptr<lua_state> lua_state;
    };
}

#endif
