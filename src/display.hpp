#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <SDL2/SDL.h>

namespace pk
{
    class display
    {
    public:
        SDL_Window *window;
        SDL_GLContext context;

        display(const char *title, int width, int height);
        ~display();

        void set_title(const char *title);
        void set_window_size(int width, int height);
        void toggle_fullscreen();

        void make_current();
        void swap();
    };
} // namespace pk

#endif
