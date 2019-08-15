#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

#include <cglm/cglm.h>

#include "material.hpp"
#include "mesh.hpp"

namespace pk
{
    struct object
    {
        pk::mesh *mesh;
        pk::material *material;
        vec3 position;
        vec3 rotation;
        vec3 scale;

        object(
            pk::mesh *mesh,
            pk::material *material,
            vec3 position,
            vec3 rotation,
            vec3 scale);
        ~object();

        void calc_model(vec4 *model);
        void draw(
            unsigned int diffuse_map_index,
            unsigned int specular_map_index,
            unsigned int normal_map_index,
            unsigned int emission_map_index) const;
    };
}

#endif
