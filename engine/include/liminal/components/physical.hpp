#ifndef LIMINAL_COMPONENTS_PHYSICAL_HPP
#define LIMINAL_COMPONENTS_PHYSICAL_HPP

#include <bullet/btBulletDynamicsCommon.h>
#include <memory>

namespace liminal
{
    class physical
    {
    public:
        btDiscreteDynamicsWorld *world;
        float mass;
        std::unique_ptr<btRigidBody> rigidbody;

        physical(btDiscreteDynamicsWorld *world, float mass = 1.0f);
        physical(physical &&other) = default;
        physical(const physical &other) = delete;
        physical &operator=(physical &&other) = default;
        physical &operator=(const physical &other) = delete;
        ~physical();
    };
}

#endif
