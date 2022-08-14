#ifndef LIMINAL_CORE_APP_HPP
#define LIMINAL_CORE_APP_HPP

#include "al_context.hpp"
#include "al_device.hpp"
#include "audio.hpp"
#include "gl_context.hpp"
#include "glew.hpp"
#include "imgui_context.hpp"
#include "renderer.hpp"
#include "sdl.hpp"
#include "sdl_image.hpp"
#include "sdl_mixer.hpp"
#include "window.hpp"
#include <memory>

namespace liminal
{
    class app
    {
    public:
        app(int argc, char *argv[]);
        virtual ~app() = default;

        void run();
        void stop();

        virtual void update(std::uint64_t current_time, float delta_time);
        virtual void resize(int width, int height);

    protected:
        std::unique_ptr<liminal::sdl> sdl;
        std::unique_ptr<liminal::sdl_image> sdl_image;
        std::unique_ptr<liminal::sdl_mixer> sdl_mixer;
        std::unique_ptr<liminal::audio> audio;
        std::unique_ptr<liminal::window> window;
        std::unique_ptr<liminal::gl_context> gl_context;
        std::unique_ptr<liminal::glew> glew;
        std::unique_ptr<liminal::imgui_context> imgui_context;
        std::unique_ptr<liminal::al_device> al_device;
        std::unique_ptr<liminal::al_context> al_context;
        std::unique_ptr<liminal::renderer> renderer;

    private:
        int window_width = 1280;
        int window_height = 720;
        float render_scale = 1;
        bool running = true;
        float time_scale = 1;
        bool console_open = false;
    };

    liminal::app *create_app(int argc, char *argv[]);
}

#endif
