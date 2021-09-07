#include "transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

liminal::transform::transform(liminal::transform *parent, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, float mass)
    : parent(parent),
      position(position),
      rotation(rotation),
      scale(scale),
      mass(mass)
{
    if (mass > 0.0f)
    {
        btTransform start_transform;
        start_transform.setIdentity();
        start_transform.setOrigin(btVector3(position.x, position.y, position.z));
        start_transform.setRotation(btQuaternion(rotation.y, rotation.x, rotation.z));

        btDefaultMotionState *motion_state = new btDefaultMotionState(start_transform);

        btCollisionShape *collision_shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));

        btVector3 local_inertia;
        collision_shape->calculateLocalInertia(mass, local_inertia);

        rigidbody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mass, motion_state, collision_shape, local_inertia));
    }
}

glm::mat4 liminal::transform::get_model_matrix() const
{
    glm::mat4 model = parent ? parent->get_model_matrix() : glm::identity<glm::mat4>();

    if (mass > 0.0f)
    {
        btTransform world_transform = rigidbody->getWorldTransform();

        glm::vec3 position = {
            world_transform.getOrigin().x(),
            world_transform.getOrigin().y(),
            world_transform.getOrigin().z()};
        model = glm::translate(model, position);

        glm::vec3 rotation;
        world_transform.getRotation().getEulerZYX(rotation.z, rotation.y, rotation.x);
        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

        model = glm::scale(model, scale);
    }
    else
    {
        model = glm::translate(model, position);

        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

        model = glm::scale(model, scale);
    }

    return model;
}
