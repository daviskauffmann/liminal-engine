#include "script.hpp"

liminal::script::script(sol::state *lua)
    : lua(lua)
{
    lua_init = (*lua)["Init"];
    lua_update = (*lua)["Update"];
}

liminal::script::~script()
{
    delete lua;
}

void liminal::script::init()
{
    lua_init();
}

void liminal::script::update(float delta_time)
{
    lua_update(delta_time);
}
