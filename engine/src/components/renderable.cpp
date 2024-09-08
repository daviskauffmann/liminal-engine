#include <liminal/components/renderable.hpp>

#include <liminal/graphics/model.hpp>

liminal::components::renderable::renderable(const std::shared_ptr<liminal::graphics::model> model)
    : model(model)
{
    if (model && model->has_animations())
    {
        bone_transformations = model->calc_bone_transformations(0, 0);
    }
}
