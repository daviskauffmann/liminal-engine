#ifndef LIMINAL_AUDIO_SOURCE_HPP
#define LIMINAL_AUDIO_SOURCE_HPP

#include <glm/vec3.hpp>
#include <liminal/audio/sound.hpp>

namespace liminal
{
    class source
    {
    public:
        source();
        ~source();

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
        unsigned int source_id;
        mutable glm::vec3 last_position;
    };
}

#endif
