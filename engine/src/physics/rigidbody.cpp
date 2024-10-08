#include <liminal/physics/rigidbody.hpp>

#include <bullet/btBulletDynamicsCommon.h>

liminal::physics::rigidbody::rigidbody(const float mass)
{
    bt_motion_state = new btDefaultMotionState();

    // TODO: more complex collision shapes
    // at least take into account transform scale for the box shape
    bt_collision_shape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));

    btVector3 local_inertia;
    bt_collision_shape->calculateLocalInertia(mass, local_inertia);

    bt_rigidbody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(
        mass,
        bt_motion_state,
        bt_collision_shape,
        local_inertia));
}

liminal::physics::rigidbody::~rigidbody()
{
    delete bt_motion_state;
    delete bt_collision_shape;
    delete bt_rigidbody;
}

btRigidBody *liminal::physics::rigidbody::get_bt_rigidbody() const
{
    return bt_rigidbody;
}

std::tuple<glm::vec3, glm::vec3> liminal::physics::rigidbody::get_world_transform() const
{
    const auto position = glm::vec3(
        bt_rigidbody->getWorldTransform().getOrigin().getX(),
        bt_rigidbody->getWorldTransform().getOrigin().getY(),
        bt_rigidbody->getWorldTransform().getOrigin().getZ());
    const auto rotation = glm::vec3(
        bt_rigidbody->getWorldTransform().getRotation().getX(),
        bt_rigidbody->getWorldTransform().getRotation().getY(),
        bt_rigidbody->getWorldTransform().getRotation().getZ());

    return std::make_tuple(position, rotation);
}

void liminal::physics::rigidbody::set_world_transform(
    const glm::vec3 &position,
    const glm::vec3 &rotation,
    const glm::vec3 &scale,
    const float mass) const
{
    btTransform world_transform;
    world_transform.setIdentity();
    world_transform.setOrigin(btVector3(position.x, position.y, position.z));
    world_transform.setRotation(btQuaternion(rotation.y, rotation.x, rotation.z));

    bt_rigidbody->setWorldTransform(world_transform);

    bt_collision_shape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));

    bt_rigidbody->setMassProps(mass, btVector3(0, 0, 0));
}
