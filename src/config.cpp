#include "config.hpp"

int window_width;
int window_height;
float render_scale;

void config_load()
{
    // TODO: load from file
    window_width = 1280;
    window_height = 720;
    render_scale = 1.0f;
}

void config_save()
{
    // TODO: save variables back to file
}
