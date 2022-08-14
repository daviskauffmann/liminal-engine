#include <liminal/components/physical.hpp>

liminal::physical::physical(btDiscreteDynamicsWorld *const world, const float mass)
    : world(world),
      mass(mass)
{
    const auto motion_state = new btDefaultMotionState();
    const auto collision_shape = new btBoxShape(btVector3(1, 1, 1));
    btVector3 local_inertia;
    collision_shape->calculateLocalInertia(mass, local_inertia);
    const auto construction_info = btRigidBody::btRigidBodyConstructionInfo(mass, motion_state, collision_shape, local_inertia);
    rigidbody = std::make_unique<btRigidBody>(construction_info);
    world->addRigidBody(rigidbody.get());
}

liminal::physical::~physical()
{
    // TODO: this causes a crash
    delete rigidbody->getMotionState();
    delete rigidbody->getCollisionShape();
    world->removeRigidBody(rigidbody.get());
}
