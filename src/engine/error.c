#include <engine/engine.h>

void error_set(const char *fmt, ...)
{
    char message[256];

    va_list ap;
    va_start(ap, fmt);
    vsprintf_s(message, sizeof(message), fmt, ap);
    va_end(ap);

    SDL_SetError(message);

    // TODO: logging?
}

const char *error_get(void)
{
    return SDL_GetError();
}
