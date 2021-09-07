#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <bullet/btBulletDynamicsCommon.h>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace liminal
{
    struct transform
    {
        liminal::transform *parent;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        float mass;
        btRigidBody *rigidbody;

        transform(liminal::transform *parent, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, float mass);

        glm::mat4 get_model_matrix() const;
    };
}

#endif
