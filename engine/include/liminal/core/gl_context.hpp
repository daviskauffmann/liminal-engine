#ifndef LIMINAL_CORE_GL_CONTEXT_HPP
#define LIMINAL_CORE_GL_CONTEXT_HPP

#include <SDL2/SDL.h>

namespace liminal
{
    class gl_context
    {
    public:
        gl_context(SDL_Window *sdl_window);
        gl_context(gl_context &&other) = delete;
        gl_context(const gl_context &other) = delete;
        gl_context &operator=(gl_context &&other) = delete;
        gl_context &operator=(const gl_context &other) = delete;
        ~gl_context();

        SDL_GLContext get_sdl_gl_context() const;

        void toggle_vsync() const;

    private:
        SDL_GLContext sdl_gl_context;
    };
}

#endif
