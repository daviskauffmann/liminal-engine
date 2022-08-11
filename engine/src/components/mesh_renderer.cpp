#include <liminal/components/mesh_renderer.hpp>

#include <liminal/graphics/model.hpp>

liminal::mesh_renderer::mesh_renderer(const liminal::model *const model, const glm::vec3 &color)
    : model(model),
      color(color)
{
    if (model && model->has_animations())
    {
        bone_transformations = model->calc_bone_transformations(0, 0);
    }
}
