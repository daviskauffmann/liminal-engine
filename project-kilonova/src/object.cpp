#include "object.hpp"

namespace pk
{
    object::object(
        pk::mesh *mesh,
        pk::material *material,
        vec3 position,
        vec3 rotation,
        vec3 scale)
    {
        this->mesh = mesh;
        this->material = material;
        glm_vec_copy(position, this->position);
        glm_vec_copy(rotation, this->rotation);
        glm_vec_copy(scale, this->scale);
    }

    object::~object()
    {

    }

    void object::calc_model(vec4 *model)
    {
        glm_translate(model, this->position);
        glm_rotate(model, this->rotation[0], vec3{ 1.0f, 0.0f, 0.0f });
        glm_rotate(model, this->rotation[1], vec3{ 0.0f, 1.0f, 0.0f });
        glm_rotate(model, this->rotation[2], vec3{ 0.0f, 0.0f, 1.0f });
        glm_scale(model, this->scale);
    }

    void object::draw(
        unsigned int diffuse_map_index,
        unsigned int specular_map_index,
        unsigned int normal_map_index,
        unsigned int emission_map_index) const
    {
        if (this->material)
        {
            this->material->bind(diffuse_map_index, specular_map_index, normal_map_index, emission_map_index);
        }

        this->mesh->draw();
    }
}
