#include <liminal/components/mesh_renderer.hpp>

liminal::mesh_renderer::mesh_renderer(liminal::model *model)
    : model(model)
{
    if (model)
    {
        model->update_bone_transformations(0, 0);
    }
}
