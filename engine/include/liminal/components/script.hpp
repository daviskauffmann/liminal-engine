#ifndef LIMINAL_COMPONENTS_SCRIPT_HPP
#define LIMINAL_COMPONENTS_SCRIPT_HPP

#include <entt/entt.hpp>
#include <memory>
#include <sol/sol.hpp>
#include <string>

namespace liminal
{
    class assets;
    class scene;

    class script
    {
    public:
        script(const std::string &filename, liminal::scene *scene, entt::entity id, std::shared_ptr<liminal::assets> assets);
        script(script &&other) = default;
        script(const script &other) = delete;
        script &operator=(script &&other) = default;
        script &operator=(const script &other) = delete;
        ~script() = default;

        void init() const;
        void update(float delta_time) const;

    private:
        // TODO: support multiple script instances in a single script component?
        // because we can't have multiple of the same component on an entity
        sol::state lua;
    };
}

#endif
