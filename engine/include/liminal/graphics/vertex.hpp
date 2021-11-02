#ifndef LIMINAL_GRAPHICS_VERTEX_HPP
#define LIMINAL_GRAPHICS_VERTEX_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#define NUM_BONES_PER_VERTEX 10

namespace liminal
{
    struct vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 tangent;
        glm::vec3 bitangent;
        unsigned int bone_ids[NUM_BONES_PER_VERTEX];
        float bone_weights[NUM_BONES_PER_VERTEX];

        void add_bone_data(unsigned int id, float weight);
    };
}

#endif
