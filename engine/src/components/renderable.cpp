#include <liminal/components/renderable.hpp>

#include <liminal/graphics/model.hpp>

liminal::renderable::renderable(const std::shared_ptr<liminal::model> model, const glm::vec3 &color)
    : model(model),
      color(color)
{
    if (model && model->has_animations())
    {
        bone_transformations = model->calc_bone_transformations(0, 0);
    }
}
