#ifndef LIMINAL_CORE_PLATFORM_HPP
#define LIMINAL_CORE_PLATFORM_HPP

#include <AL/alc.h>
#include <SDL2/SDL.h>

namespace liminal
{
    class platform
    {
    public:
        static platform *instance;

        platform(const char *window_title, int window_width, int window_height);
        ~platform();

        void set_window_title(const char *title) const;
        void set_window_size(int width, int height) const;
        void toggle_fullscreen() const;
        void toggle_vsync() const;
        bool get_relative_mouse_mode() const;
        void set_relative_mouse_mode(bool enabled) const;

        void process_event(const SDL_Event *const event) const;

        void begin_frame() const;
        void end_frame() const;

    private:
        SDL_Window *window = nullptr;
        SDL_GLContext context = nullptr;
        ALCdevice *al_device = nullptr;
        ALCcontext *al_context = nullptr;
    };
}

#endif
