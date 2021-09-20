#ifndef PHYSICAL_HPP
#define PHYSICAL_HPP

#include <bullet/btBulletDynamicsCommon.h>

namespace liminal
{
    struct physical
    {
        float mass;
        btRigidBody *rigidbody;

        physical(float mass);
    };
}

#endif
