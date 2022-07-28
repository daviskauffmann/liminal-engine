#ifndef LIMINAL_COMPONENTS_AUDIO_SOURCE_HPP
#define LIMINAL_COMPONENTS_AUDIO_SOURCE_HPP

#include <AL/al.h>
#include <glm/vec3.hpp>
#include <liminal/audio/sound.hpp>

namespace liminal
{
    class audio_source
    {
    public:
        audio_source();
        ~audio_source();

        void set_loop(const bool loop) const;
        void set_gain(const float gain) const;
        void set_pitch(const float pitch) const;
        void set_position(const glm::vec3 &position);
        bool is_playing() const;
        void play(const liminal::sound &sound) const;
        void pause() const;
        void resume() const;
        void stop() const;

    private:
        ALuint source_id;
        glm::vec3 last_position = {};
    };
}

#endif
