#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <bullet/btBulletDynamicsCommon.h>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

#include "model.hpp"

namespace pk
{
    struct object
    {
        pk::model *model;
        btRigidBody *rigidbody;

        object(
            pk::model *model,
            glm::vec3 position,
            glm::vec3 rotation,
            glm::vec3 scale,
            float mass);
        ~object();

        glm::mat4 calc_model() const;

    private:
        glm::vec3 scale;
        btDefaultMotionState *motion_state;
        btCollisionShape *collision_shape;
    };
} // namespace pk

#endif
