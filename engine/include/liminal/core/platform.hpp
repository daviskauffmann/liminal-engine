#ifndef LIMINAL_CORE_PLATFORM_HPP
#define LIMINAL_CORE_PLATFORM_HPP

#include <AL/alc.h>
#include <SDL2/SDL.h>
#include <string>

namespace liminal
{
    class platform
    {
    public:
        static platform *instance;

        platform(const std::string &window_title, int window_width, int window_height);
        ~platform();

        void set_window_title(const std::string &title);
        void set_window_size(int width, int height);
        void toggle_fullscreen();
        void toggle_vsync();
        bool get_relative_mouse_mode();
        void set_relative_mouse_mode(bool enabled);

        void process_event(SDL_Event *event);

        void begin_frame();
        void end_frame();

    private:
        SDL_Window *window;
        SDL_GLContext context;
        ALCdevice *al_device;
        ALCcontext *al_context;
    };
}

#endif
