#ifndef LIMINAL_COMPONENTS_PHYSICAL_HPP
#define LIMINAL_COMPONENTS_PHYSICAL_HPP

#include <bullet/btBulletDynamicsCommon.h>
#include <memory>

namespace liminal
{
    class physical
    {
    public:
        float mass = 1.0f;
        std::shared_ptr<btRigidBody> rigidbody;
    };
}

#endif
