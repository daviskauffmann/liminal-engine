#include "display.hpp"

#include <GL/glew.h>
#include <iostream>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <string>

namespace pk
{
display::display(const char *title, int width, int height)
{
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    SDL_version sdl_version;
    SDL_GetVersion(&sdl_version);
    std::cout << "SDL " << std::to_string(sdl_version.major) << "." << std::to_string(sdl_version.minor) << "." << std::to_string(sdl_version.patch) << std::endl;

    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    const SDL_version *img_version = IMG_Linked_Version();
    std::cout << "SDL_image " << std::to_string(img_version->major) << "." << std::to_string(img_version->minor) << "." << std::to_string(img_version->patch) << std::endl;

    Mix_Init(0);
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);
    const SDL_version *mix_version = Mix_Linked_Version();
    std::cout << "SDL_mixer " << std::to_string(mix_version->major) << "." << std::to_string(mix_version->minor) << "." << std::to_string(mix_version->patch) << std::endl;

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    context = SDL_GL_CreateContext(window);

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        std::cout << "Error: Couldn't initialize GLEW\n"
                  << glewGetErrorString(error) << std::endl;
    }
    std::cout << "GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Vendor " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

display::~display()
{
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);

    Mix_Quit();
    Mix_CloseAudio();

    IMG_Quit();

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

void display::make_current()
{
    SDL_GL_MakeCurrent(window, context);
}

void display::swap()
{
    SDL_GL_SwapWindow(window);
}
} // namespace pk
