#include <liminal/core/app.hpp>

#include <liminal/graphics/renderer.hpp>

void liminal::app::resize(int width, int height)
{
    liminal::renderer::instance->set_target_size(width, height);
}
