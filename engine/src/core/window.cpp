#include <liminal/core/window.hpp>

#include <stdexcept>

liminal::window::window(const char *const title, const int width, const int height)
{
    sdl_window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!sdl_window)
    {
        throw std::runtime_error(SDL_GetError());
    }
}

liminal::window::~window()
{
    SDL_DestroyWindow(sdl_window);
}

SDL_Window *liminal::window::get_sdl_window() const
{
    return sdl_window;
}

void liminal::window::set_title(const char *const title) const
{
    SDL_SetWindowTitle(sdl_window, title);
}

void liminal::window::set_size(const int width, const int height) const
{
    SDL_SetWindowSize(sdl_window, width, height);
}

void liminal::window::toggle_fullscreen() const
{
    const auto flags = SDL_GetWindowFlags(sdl_window);
    if (flags & SDL_WINDOW_FULLSCREEN)
    {
        SDL_SetWindowFullscreen(sdl_window, 0);
    }
    else
    {
        SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN);
    }
}

void liminal::window::swap() const
{
    SDL_GL_SwapWindow(sdl_window);
}
