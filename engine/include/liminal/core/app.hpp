#ifndef LIMINAL_CORE_APP_HPP
#define LIMINAL_CORE_APP_HPP

#include <liminal/core/scene.hpp>

// TODO: app shouldn't own a scene like this
// probably just wanna make the physics world and entt registry just part of the engine and accessible globally through some handy api

namespace liminal
{
    class app
    {
    public:
        liminal::scene *scene;

        virtual ~app() {}

        virtual void update(unsigned int current_time, float delta_time) {}
    };

    liminal::app *create_app(int argc, char *argv[]);
}

#endif
