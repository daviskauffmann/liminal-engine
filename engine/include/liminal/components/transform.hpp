#ifndef LIMINAL_COMPONENTS_TRANSFORM_HPP
#define LIMINAL_COMPONENTS_TRANSFORM_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <string>

namespace liminal
{
    struct transform
    {
        std::string name;
        liminal::transform *parent;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        glm::mat4 get_model_matrix() const;
    };
}

#endif
