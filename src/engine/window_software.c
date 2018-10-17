#include <engine/engine.h>

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *screen;

int window_software_init(const char *title, int width, int height)
{
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        0);

    if (!window)
    {
        error_set(SDL_GetError());

        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer)
    {
        error_set(SDL_GetError());

        return 1;
    }

    screen = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height);

    if (!screen)
    {
        printf("Error: %s\n", SDL_GetError());

        return 1;
    }

    return 0;
}

const char *window_software_get_title(void)
{
    return SDL_GetWindowTitle(window);
}

void window_software_set_title(const char *title)
{
    SDL_SetWindowTitle(window, title);
}

void window_software_toggle_fullscreen(void)
{
    unsigned int flags = SDL_GetWindowFlags(window);

    if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
    {
        SDL_SetWindowFullscreen(window, 0);
    }
    else
    {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
}

void window_software_clear(void)
{
    SDL_RenderClear(renderer);
}

void window_software_draw_pixels(SDL_Rect *rect, const void *pixels, int pitch)
{
    SDL_UpdateTexture(
        screen,
        NULL,
        pixels,
        pitch);

    SDL_RenderCopy(renderer, screen, NULL, NULL);
}

void window_software_draw_text(TTF_Font *font, int px, int x, int y, SDL_Color fg, const char *const fmt, ...)
{
    char text[256];

    va_list ap;
    va_start(ap, fmt);

    vsprintf_s(text, sizeof(text), fmt, ap);

    va_end(ap);

    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, fg);

    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    SDL_FreeSurface(text_surface);

    SDL_Rect text_rect;
    text_rect.x = x;
    text_rect.y = y;
    text_rect.w = px * strlen(text);
    text_rect.h = px;

    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    SDL_DestroyTexture(text_texture);
}

void window_software_render(void)
{
    SDL_RenderPresent(renderer);
}

void window_software_quit(void)
{
    SDL_DestroyTexture(screen);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}
