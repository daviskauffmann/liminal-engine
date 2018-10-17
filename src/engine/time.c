#include <engine/engine.h>

static unsigned int frame_delay = 0;
static unsigned int frame_start = 0;
static unsigned int current = 0;
static float delta = 0.0f;
static float fps_update_timer = 0.0f;
static unsigned int fps = 0;

void time_cap_fps(unsigned int fps_cap)
{
    frame_delay = 1000 / fps_cap;
}

void time_frame_start(void)
{
    frame_start = SDL_GetTicks();

    unsigned int previous = current;
    current = SDL_GetTicks();
    delta = (current - previous) / 1000.0f;

    fps_update_timer += delta;
    if (fps_update_timer >= 0.25f)
    {
        fps_update_timer = 0.0f;
        fps = (unsigned int)(1 / delta);
    }
}

void time_frame_end(void)
{
    unsigned int frame_end = SDL_GetTicks();
    unsigned int frame_time = frame_end - frame_start;

    if (frame_delay > frame_time)
    {
        SDL_Delay(frame_delay - frame_time);
    }
}

unsigned int time_current(void)
{
    return current;
}

float time_delta(void)
{
    return delta;
}

unsigned int time_fps(void)
{
    return fps;
}
