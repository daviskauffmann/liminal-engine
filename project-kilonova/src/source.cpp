#include "source.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace pk
{
source::source()
{
    alGenSources(1, &this->source_id);
}

source::~source()
{
    alDeleteSources(1, &this->source_id);
}

void source::set_loop(bool loop) const
{
    alSourcei(this->source_id, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

void source::set_gain(float gain) const
{
    alSourcef(this->source_id, AL_GAIN, gain);
}

void source::set_pitch(float pitch) const
{
    alSourcef(this->source_id, AL_PITCH, pitch);
}

void source::set_position(glm::vec3 position) const
{
    glm::vec3 velocity = this->position - position;
    alSourcefv(this->source_id, AL_POSITION, glm::value_ptr(position));
    alSourcefv(this->source_id, AL_VELOCITY, glm::value_ptr(velocity));
    this->position = position;
}

bool source::is_playing() const
{
    ALint value;
    alGetSourcei(this->source_id, AL_SOURCE_STATE, &value);
    return value == AL_PLAYING;
}

void source::play(pk::sound *sound) const
{
    alSourcei(this->source_id, AL_BUFFER, sound->buffer_id);
    alSourcePlay(this->source_id);
}

void source::resume() const
{
    alSourcePlay(this->source_id);
}

void source::pause() const
{
    alSourcePause(this->source_id);
}

void source::stop() const
{
    alSourceStop(this->source_id);
}
} // namespace pk
