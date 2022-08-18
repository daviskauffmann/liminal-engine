#ifndef LIMINAL_AUDIO_SOURCE_HPP
#define LIMINAL_AUDIO_SOURCE_HPP

#include <AL/al.h>
#include <glm/vec3.hpp>

namespace liminal
{
    class sound;

    class source
    {
    public:
        source();        
        source(source &&other) = delete;
        source(const source &other) = delete;
        source &operator=(source &&other) = delete;
        source &operator=(const source &other) = delete;
        ~source();

        void set_loop(bool loop) const;
        void set_gain(float gain) const;
        void set_pitch(float pitch) const;
        void set_position(const glm::vec3 &position) const;
        void set_velocity(const glm::vec3 &velocity) const;
        bool is_playing() const;
        void play(const liminal::sound &sound) const;
        void pause() const;
        void resume() const;
        void stop() const;

    private:
        ALuint source_id;
    };
}

#endif
