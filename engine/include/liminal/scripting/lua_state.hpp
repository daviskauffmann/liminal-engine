#ifndef LIMINAL_SCRIPTING_LUA_STATE_HPP
#define LIMINAL_SCRIPTING_LUA_STATE_HPP

#include <entt/entt.hpp>
#include <sol/sol.hpp>

namespace liminal
{
    namespace core
    {
        class assets;
    }

    namespace entities
    {
        class scene;
    }

    namespace scripting
    {
        class lua_state
        {
        public:
            lua_state(const std::string &filename, liminal::entities::scene *scene, entt::entity id, std::shared_ptr<liminal::core::assets> assets);
            ~lua_state() = default;
            lua_state(const lua_state &other) = delete;
            lua_state &operator=(const lua_state &other) = delete;
            lua_state(lua_state &&other) = default;
            lua_state &operator=(lua_state &&other) = default;

            void init() const;
            void update(float delta_time) const;

        private:
            sol::state lua;
        };
    }
}

#endif
