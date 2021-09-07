#include "engine.hpp"

liminal::engine::engine(const std::string &window_title, int window_width, int window_height, float render_scale)
{
    platform = new liminal::platform(window_title, window_width, window_height);
    renderer = new liminal::renderer(
        window_width, window_height, render_scale,
        window_width, window_height,
        window_width, window_height);
    audio = new liminal::audio();
}

liminal::engine::~engine()
{
    delete platform;
    delete renderer;
    delete audio;
}
