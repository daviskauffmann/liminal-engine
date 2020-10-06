#include "source.hpp"

#include <glm/gtc/type_ptr.hpp>

pk::source::source(glm::vec3 position)
{
    alGenSources(1, &source_id);
    this->position = position;

    alSourcefv(source_id, AL_POSITION, glm::value_ptr(position));
}

pk::source::~source()
{
    alDeleteSources(1, &source_id);
}

void pk::source::set_loop(bool loop) const
{
    alSourcei(source_id, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

void pk::source::set_gain(float gain) const
{
    alSourcef(source_id, AL_GAIN, gain);
}

void pk::source::set_pitch(float pitch) const
{
    alSourcef(source_id, AL_PITCH, pitch);
}

void pk::source::set_position(glm::vec3 position) const
{
    glm::vec3 velocity = this->position - position;
    alSourcefv(source_id, AL_POSITION, glm::value_ptr(position));
    alSourcefv(source_id, AL_VELOCITY, glm::value_ptr(velocity));
    this->position = position;
}

bool pk::source::is_playing() const
{
    ALint state;
    alGetSourcei(source_id, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

void pk::source::play(pk::sound *sound) const
{
    alSourcei(source_id, AL_BUFFER, sound->buffer_id);
    alSourcePlay(source_id);
}

void pk::source::resume() const
{
    alSourcePlay(source_id);
}

void pk::source::pause() const
{
    alSourcePause(source_id);
}

void pk::source::stop() const
{
    alSourceStop(source_id);
}
