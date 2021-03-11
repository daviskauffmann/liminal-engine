#ifndef WATER_HPP
#define WATER_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace liminal
{
    struct water
    {
        glm::vec3 position;
        glm::vec2 scale;

        water(glm::vec3 position, glm::vec2 scale);
    };
} // namespace liminal

#endif
