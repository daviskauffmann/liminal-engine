#include "material.hpp"

namespace pk
{
    material::material(
        glm::vec3 color,
        pk::texture *diffuse_map,
        pk::texture *specular_map,
        float shininess,
        pk::texture *normal_map,
        pk::texture *emission_map,
        float glow)
    {
        this->color = color;
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
