#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

namespace pk
{
    struct material;
    struct mesh;

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
