#ifndef LIMINAL_CORE_WINDOW_HPP
#define LIMINAL_CORE_WINDOW_HPP

#include <SDL2/SDL.h>
#include <memory>

namespace liminal
{
    class window
    {
    public:
        window(const char *title, int width, int height);
        window(window &&other) = delete;
        window(const window &other) = delete;
        window &operator=(window &&other) = delete;
        window &operator=(const window &other) = delete;
        ~window();

        SDL_Window *get_sdl_window() const;

        void set_title(const char *title) const;
        void set_size(int width, int height) const;
        void toggle_fullscreen() const;

        void swap() const;

    private:
        SDL_Window *sdl_window;
    };
}

#endif