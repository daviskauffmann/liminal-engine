#include <liminal/physics/rigidbody.hpp>

#include <bullet/btBulletDynamicsCommon.h>

liminal::rigidbody::rigidbody(
    const glm::vec3 &scale,
    const float mass)
{
    // TODO: more complex collision shapes
    // at least take into account transform scale for the box shape
    const auto motion_state = new btDefaultMotionState();
    const auto collision_shape = new btBoxShape(btVector3(scale.x, scale.y, scale.z));
    btVector3 local_inertia;
    collision_shape->calculateLocalInertia(mass, local_inertia);
    const auto construction_info = btRigidBody::btRigidBodyConstructionInfo(mass, motion_state, collision_shape, local_inertia);
    bt_rigidbody = new btRigidBody(construction_info);
}

liminal::rigidbody::~rigidbody()
{
    delete bt_rigidbody->getMotionState();
    delete bt_rigidbody->getCollisionShape();
    delete bt_rigidbody;
}

btRigidBody *liminal::rigidbody::get_bt_rigidbody() const
{
    return bt_rigidbody;
}

std::tuple<glm::vec3, glm::vec3> liminal::rigidbody::get_world_transform() const
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

void liminal::rigidbody::set_world_transform(const glm::vec3 &position, const glm::vec3 &rotation) const
{
    btTransform world_transform;
    world_transform.setIdentity();
    world_transform.setOrigin(btVector3(position.x, position.y, position.z));
    world_transform.setRotation(btQuaternion(rotation.y, rotation.x, rotation.z));
    bt_rigidbody->setWorldTransform(world_transform);
}
