#include "object.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace pk
{
object::object(
    pk::mesh *mesh,
    pk::material *material,
    glm::vec3 position,
    glm::vec3 rotation,
    glm::vec3 scale)
    : mesh(mesh), material(material), position(position), rotation(rotation), scale(scale)
{
}

object::~object()
{
}

glm::mat4 object::calc_model() const
{
    glm::mat4 model(1.0f);

    model = glm::translate(model, this->position);
    model = glm::rotate(model, this->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, this->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, this->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, this->scale);

    return model;
}

void object::draw(
    unsigned int albedo_map_index,
    unsigned int normal_map_index,
    unsigned int metallic_map_index,
    unsigned int roughness_map_index,
    unsigned int ao_map_index) const
{
    if (this->material)
    {
        this->material->bind(
            albedo_map_index,
            normal_map_index,
            metallic_map_index,
            roughness_map_index,
            ao_map_index);
    }

    this->mesh->draw();
}
} // namespace pk
