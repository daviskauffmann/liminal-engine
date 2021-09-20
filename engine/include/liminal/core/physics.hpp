#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include <liminal/core/scene.hpp>

class btDiscreteDynamicsWorld;

namespace liminal
{
    class physics
    {
    public:
        btDiscreteDynamicsWorld *world;

        physics();
        ~physics();

        void update(liminal::scene &scene, float delta_time);
    };
}

#endif
