#include "display.hpp"

#include <iostream>
#include <string>

namespace pk
{
display::display(const char *title, int width, int height)
{
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    SDL_version version;
    SDL_GetVersion(&version);
    std::cout << "SDL " << std::to_string(version.major) << "." << std::to_string(version.minor) << "." << std::to_string(version.patch) << std::endl;

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    context = SDL_GL_CreateContext(window);
}

display::~display()
{
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

void display::set_title(const char *title)
{
    SDL_SetWindowTitle(window, title);
}

void display::set_window_size(int width, int height)
{
    SDL_SetWindowSize(window, width, height);
}

void display::toggle_fullscreen()
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

void display::swap()
{
    SDL_GL_SwapWindow(window);
}
} // namespace pk
