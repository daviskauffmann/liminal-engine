#ifndef GAME_MATERIAL_HPP
#define GAME_MATERIAL_HPP

#include <glm/vec3.hpp>

#include "texture.hpp"

namespace pk
{
    struct material
    {
        glm::vec3 color;
        pk::texture *diffuse_map;
        pk::texture *specular_map;
        float shininess;
        pk::texture *normal_map;
        pk::texture *emission_map;
        float glow;

        material(
            glm::vec3 color,
            pk::texture *diffuseMap,
            pk::texture *specularMap,
            float shininess,
            pk::texture *normalMap,
            pk::texture *emissionMap,
            float glow);
        ~material();

        void bind(
            unsigned int diffuse_map_index,
            unsigned int specular_map_index,
            unsigned int normal_map_index,
            unsigned int emission_map_index) const;
    };
}

#endif
