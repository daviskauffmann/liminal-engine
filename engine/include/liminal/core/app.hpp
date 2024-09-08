#ifndef LIMINAL_CORE_APP_HPP
#define LIMINAL_CORE_APP_HPP

#include "al_context.hpp"
#include "al_device.hpp"
#include "assets.hpp"
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
    namespace core
    {
        class app
        {
        public:
            app(int argc, char *argv[]);
            virtual ~app() = default;
            app(const app &other) = delete;
            app &operator=(const app &other) = delete;
            app(app &&other) = delete;
            app &operator=(app &&other) = delete;

            void run();
            void stop();

            virtual void update(std::uint64_t current_time, float delta_time);
            virtual void resize(int width, int height);

        protected:
            std::unique_ptr<liminal::core::sdl> sdl;
            std::unique_ptr<liminal::core::sdl_image> sdl_image;
            std::unique_ptr<liminal::core::sdl_mixer> sdl_mixer;
            std::unique_ptr<liminal::core::audio> audio;
            std::unique_ptr<liminal::core::window> window;
            std::unique_ptr<liminal::core::gl_context> gl_context;
            std::unique_ptr<liminal::core::glew> glew;
            std::unique_ptr<liminal::core::imgui_context> imgui_context;
            std::unique_ptr<liminal::core::al_device> al_device;
            std::unique_ptr<liminal::core::al_context> al_context;
            std::shared_ptr<liminal::core::assets> assets;
            std::unique_ptr<liminal::core::renderer> renderer;

        private:
            bool running = true;
            float time_scale = 1;
            bool console_open = false;
        };

        liminal::core::app *create_app(int argc, char *argv[]);
    }
}

#endif
