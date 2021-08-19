#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <SDL2/SDL.h>

namespace liminal
{
    class platform
    {
    public:
        platform(const char *title, int width, int height);
        ~platform();

        void set_window_size(int width, int height);
        void toggle_fullscreen();

        void process_event(SDL_Event *event);

        void begin_render();
        void end_render();

    private:
        SDL_Window *window;
        SDL_GLContext context;
    };
}

#endif
