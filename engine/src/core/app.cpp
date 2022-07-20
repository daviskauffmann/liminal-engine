#include <liminal/core/app.hpp>

#include <liminal/core/engine.hpp>
#include <liminal/graphics/renderer.hpp>

void liminal::app::resize(int width, int height)
{
    liminal::engine::instance->renderer->set_target_size(width, height);
}
