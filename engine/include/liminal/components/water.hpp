#ifndef WATER_HPP
#define WATER_HPP

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

namespace liminal
{
    struct water
    {
        glm::vec3 position; // TODO: maybe just use transform component for water
        float size;

        glm::mat4 get_model_matrix() const;
    };
}

#endif
