#include <liminal/components/physical.hpp>

#include <bullet/btBulletDynamicsCommon.h>
#include <liminal/core/engine.hpp>

liminal::physical::physical(float mass) : mass(mass)
{
    btDefaultMotionState *motion_state = new btDefaultMotionState();
    btCollisionShape *collision_shape = new btBoxShape(btVector3(1, 1, 1));
    btVector3 local_inertia;
    collision_shape->calculateLocalInertia(mass, local_inertia);
    rigidbody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mass, motion_state, collision_shape, local_inertia));
}
