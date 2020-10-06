#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <SDL2/SDL.h>

namespace pk
{
    class display
    {
    public:
        display(const char *title, int width, int height);
        ~display();

        bool poll_event(SDL_Event *event);

        void set_title(const char *title);
        void set_window_size(int width, int height);
        void toggle_fullscreen();

        void make_current();
        void start_gui();
        void end_gui();
        void swap();

    private:
        SDL_Window *window;
        SDL_GLContext context;
    };
} // namespace pk

#endif
