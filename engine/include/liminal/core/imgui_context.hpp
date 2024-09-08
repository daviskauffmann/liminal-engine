#ifndef LIMINAL_CORE_IMGUI_CONTEXT_HPP
#define LIMINAL_CORE_IMGUI_CONTEXT_HPP

#include <SDL2/SDL.h>

namespace liminal
{
    namespace core
    {
        class imgui_context
        {
        public:
            imgui_context(SDL_Window *sdl_window, SDL_GLContext sdl_gl_context);
            ~imgui_context();
            imgui_context(const imgui_context &other) = delete;
            imgui_context &operator=(const imgui_context &other) = delete;
            imgui_context(imgui_context &&other) = delete;
            imgui_context &operator=(imgui_context &&other) = delete;

            void handle_event(const SDL_Event &event) const;

            void begin_frame() const;
            void end_frame() const;

        private:
            SDL_Window *const sdl_window;
            void *const sdl_gl_context;
        };
    }
}

#endif
