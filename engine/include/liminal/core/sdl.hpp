#ifndef LIMINAL_CORE_PLATFORM_HPP
#define LIMINAL_CORE_PLATFORM_HPP

#include <SDL2/SDL.h>
#include <cstdint>

namespace liminal
{
    namespace core
    {
        class sdl
        {
        public:
            sdl();
            ~sdl();
            sdl(const sdl &other) = delete;
            sdl &operator=(const sdl &other) = delete;
            sdl(sdl &&other) = delete;
            sdl &operator=(sdl &&other) = delete;

            std::uint64_t get_ticks() const;
            const std::uint8_t *get_keys() const;
            std::uint32_t get_mouse_state(int *x, int *y) const;

            bool get_relative_mouse_mode() const;
            void set_relative_mouse_mode(bool enabled) const;

            bool poll_event(SDL_Event &event);
        };
    }
}

#endif
