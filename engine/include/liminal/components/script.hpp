#ifndef LIMINAL_COMPONENTS_SCRIPT_HPP
#define LIMINAL_COMPONENTS_SCRIPT_HPP

#include <memory>
#include <string>

namespace liminal
{
    namespace scripting
    {
        class lua_state;
    }

    namespace components
    {
        struct script
        {
            std::string filename;
            std::shared_ptr<liminal::scripting::lua_state> lua_state;
        };
    }
}

#endif
