#include <game/game.hpp>

namespace pk
{
    material::material(
        vec3 _color,
        pk::texture *diffuseMap,
        pk::texture *specularMap,
        float shininess,
        pk::texture *normalMap,
        pk::texture *emissionMap,
        float glow)
    {
        glm_vec_copy(color, this->color);
        this->diffuse_map = diffuse_map;
        this->specular_map = specular_map;
        this->shininess = shininess;
        this->normal_map = normal_map;
        this->emission_map = emission_map;
        this->glow = glow;
    }

    material::~material()
    {

    }

    void material::bind(
        unsigned int diffuse_map_index,
        unsigned int specular_map_index,
        unsigned int normal_map_index,
        unsigned int emission_map_index) const
    {
        if (this->diffuse_map)
        {
            this->diffuse_map->bind(diffuse_map_index);
        }

        if (this->specular_map)
        {
            this->specular_map->bind(specular_map_index);
        }

        if (this->normal_map)
        {
            this->normal_map->bind(normal_map_index);
        }

        if (this->emission_map)
        {
            this->emission_map->bind(emission_map_index);
        }
    }
}
