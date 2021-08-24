#ifndef PHYSICAL_HPP
#define PHYSICAL_HPP

#include <bullet/btBulletDynamicsCommon.h>

namespace liminal
{
    struct physical
    {
        btRigidBody *rigidbody;

        physical(float mass);
        ~physical();

        btDefaultMotionState *motion_state;
        btCollisionShape *collision_shape;
    };
}

#endif
