#include <liminal/components/physical.hpp>

#include <bullet/btBulletDynamicsCommon.h>

liminal::physical::physical(float mass) : mass(mass)
{
    const auto motion_state = new btDefaultMotionState();
    const auto collision_shape = new btBoxShape(btVector3(1, 1, 1));
    btVector3 local_inertia;
    collision_shape->calculateLocalInertia(mass, local_inertia);
    const auto construction_info = btRigidBody::btRigidBodyConstructionInfo(mass, motion_state, collision_shape, local_inertia);
    rigidbody = new btRigidBody(construction_info);
}

liminal::physical::~physical()
{
    delete rigidbody;
}
