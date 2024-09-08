#include <liminal/core/gl_context.hpp>

#include <stdexcept>

liminal::core::gl_context::gl_context(SDL_Window *const sdl_window)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetSwapInterval(1);

    sdl_gl_context = SDL_GL_CreateContext(sdl_window);
    if (!sdl_gl_context)
    {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_GL_MakeCurrent(sdl_window, sdl_gl_context);
}

liminal::core::gl_context::~gl_context()
{
    SDL_GL_DeleteContext(sdl_gl_context);
}

SDL_GLContext liminal::core::gl_context::get_sdl_gl_context() const
{
    return sdl_gl_context;
}

void liminal::core::gl_context::toggle_vsync() const
{
    SDL_GL_SetSwapInterval(SDL_GL_GetSwapInterval() ? 0 : 1);
}
