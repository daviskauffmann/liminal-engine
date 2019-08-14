#ifndef GAME_SOURCE_HPP
#define GAME_SOURCE_HPP

namespace pk
{
    struct sound;

    class source
    {
    public:
        source();
        ~source();

        void set_loop(bool loop) const;
        void set_gain(float gain) const;
        void set_pitch(float pitch) const;
        void set_position(vec3 position) const;
        void set_velocity(vec3 velocity) const;
        bool is_playing() const;
        void play(pk::sound *sound) const;
        void pause() const;
        void resume() const;
        void stop() const;
    private:
        ALuint source_id;
    };
}

#endif
