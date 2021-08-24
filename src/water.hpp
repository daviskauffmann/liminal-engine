#ifndef WATER_HPP
#define WATER_HPP

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

namespace liminal
{
    struct water
    {
        glm::vec3 position;
        float size;

        water(glm::vec3 position, float size);

        glm::mat4 calc_model() const;
    };
}

#endif
