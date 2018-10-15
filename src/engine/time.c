#include "time.h"

unsigned int current = 0;
float delta = 0.0f;

void time_update(void)
{
    unsigned int previous = current;
    current = SDL_GetTicks();
    delta = (current - previous) / 1000.0f;
}

unsigned int time_current(void)
{
    return current;
}

float time_delta(void)
{
    return delta;
}
