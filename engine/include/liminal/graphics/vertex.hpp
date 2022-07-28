#ifndef LIMINAL_GRAPHICS_VERTEX_HPP
#define LIMINAL_GRAPHICS_VERTEX_HPP

#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace liminal
{
    class vertex
    {
    public:
        static constexpr std::size_t num_bones = 4;

        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 tangent;
        glm::vec3 bitangent;
        std::array<unsigned int, num_bones> bone_ids;
        std::array<float, num_bones> bone_weights;

        void add_bone_data(unsigned int id, float weight);
    };
}

#endif
