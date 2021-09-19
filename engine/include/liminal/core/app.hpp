#ifndef APP_HPP
#define APP_HPP

#include <liminal/core/scene.hpp>
#include <liminal/graphics/renderer.hpp>

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
