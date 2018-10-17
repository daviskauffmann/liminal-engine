#include <engine/engine.h>

// TODO: logging to file

void error(const char *fmt, ...)
{
    char message[1024];

    va_list ap;
    va_start(ap, fmt);
    vsprintf_s(message, sizeof(message), fmt, ap);
    va_end(ap);

    printf("Error: %s\n", message);
}

void warning(const char *fmt, ...)
{
    char message[1024];

    va_list ap;
    va_start(ap, fmt);
    vsprintf_s(message, sizeof(message), fmt, ap);
    va_end(ap);

    printf("Warning: %s\n", message);
}

void info(const char *fmt, ...)
{
    char message[1024];

    va_list ap;
    va_start(ap, fmt);
    vsprintf_s(message, sizeof(message), fmt, ap);
    va_end(ap);

    printf("%s\n", message);
}
