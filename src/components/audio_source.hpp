#ifndef SOURCE_HPP
#define SOURCE_HPP

#include <glm/vec3.hpp>

#include "../sound.hpp"

namespace liminal
{
    class audio_source
    {
    public:
        audio_source();
        ~audio_source();

        void set_loop(bool loop) const;
        void set_gain(float gain) const;
        void set_pitch(float pitch) const;
        void set_position(glm::vec3 position) const;
        bool is_playing() const;
        void play(const liminal::sound &sound) const;
        void pause() const;
        void resume() const;
        void stop() const;

    private:
        ALuint source_id;
        mutable glm::vec3 last_position;
    };
}

#endif
