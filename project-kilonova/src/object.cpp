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
    //model = glm::rotate(model, );
    model = glm::scale(model, this->scale);

    return model;
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
} // namespace pk
