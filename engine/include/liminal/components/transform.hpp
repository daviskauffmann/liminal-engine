#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace liminal
{
    struct transform
    {
        const char *name;
        liminal::transform *parent;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        glm::mat4 get_model_matrix() const;
    };
}

#endif
