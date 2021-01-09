#include "object.hpp"

#include <glm/gtc/matrix_transform.hpp>

pk::object::object(
    pk::model *model,
    glm::vec3 position,
    glm::vec3 rotation,
    glm::vec3 scale,
    float mass)
    : model(model)
{
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(position.x, position.y, position.z));
    transform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z));

    motion_state = new btDefaultMotionState(transform);

    collision_shape = new btBoxShape(btVector3(scale.x, scale.y, scale.z));
    btVector3 local_intertia(0.0f, 0.0f, 0.0f);
    if (mass != 0.f)
    {
        collision_shape->calculateLocalInertia(mass, local_intertia);
    }

    rigidbody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mass, motion_state, collision_shape, local_intertia));
}

pk::object::~object()
{
    delete motion_state;
    delete collision_shape;
}

glm::mat4 pk::object::calc_model() const
{
    glm::mat4 model = glm::identity<glm::mat4>();
    btTransform transform = rigidbody->getWorldTransform();

    glm::vec3 position = {
        transform.getOrigin().x(),
        transform.getOrigin().y(),
        transform.getOrigin().z()};
    model = glm::translate(model, position);

    glm::vec3 rotation;
    transform.getRotation().getEulerZYX(rotation.z, rotation.y, rotation.x);
    model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    btVector3 scale = collision_shape->getLocalScaling();
    model = glm::scale(model, glm::vec3(scale.x(), scale.y(), scale.z()));
    return model;
}
