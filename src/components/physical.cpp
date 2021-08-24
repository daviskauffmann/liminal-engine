#include "physical.hpp"

liminal::physical::physical(float mass)
{
    motion_state = new btDefaultMotionState();

    collision_shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
    btVector3 local_inertia(0.0f, 0.0f, 0.0f);
    if (mass != 0.f)
    {
        collision_shape->calculateLocalInertia(mass, local_inertia);
    }

    rigidbody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mass, motion_state, collision_shape, local_inertia));
}

liminal::physical::~physical()
{
    delete motion_state;
    delete collision_shape;
}

// glm::mat4 liminal::object::calc_model() const
// {
//     glm::mat4 model = glm::identity<glm::mat4>();
//     btTransform transform = rigidbody->getWorldTransform();

//     glm::vec3 position = {
//         transform.getOrigin().x(),
//         transform.getOrigin().y(),
//         transform.getOrigin().z()};
//     model = glm::translate(model, position);

//     glm::vec3 rotation;
//     transform.getRotation().getEulerZYX(rotation.z, rotation.y, rotation.x);
//     model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
//     model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
//     model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

//     model = glm::scale(model, scale);
//     return model;
// }
