#include "audio_source.hpp"

#include <glm/gtc/type_ptr.hpp>

liminal::audio_source::audio_source()
{
    alGenSources(1, &source_id);
    this->last_position = glm::vec3(0.0f);

    alSourcefv(source_id, AL_POSITION, glm::value_ptr(this->last_position));
}

liminal::audio_source::~audio_source()
{
    alDeleteSources(1, &source_id);
}

void liminal::audio_source::set_loop(bool loop) const
{
    alSourcei(source_id, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

void liminal::audio_source::set_gain(float gain) const
{
    alSourcef(source_id, AL_GAIN, gain);
}

void liminal::audio_source::set_pitch(float pitch) const
{
    alSourcef(source_id, AL_PITCH, pitch);
}

void liminal::audio_source::set_position(glm::vec3 position) const
{
    glm::vec3 velocity = this->last_position - position;

    alSourcefv(source_id, AL_POSITION, glm::value_ptr(position));
    alSourcefv(source_id, AL_VELOCITY, glm::value_ptr(velocity));

    this->last_position = position;
}

bool liminal::audio_source::is_playing() const
{
    ALint state;
    alGetSourcei(source_id, AL_SOURCE_STATE, &state);

    return state == AL_PLAYING;
}

void liminal::audio_source::play(const liminal::sound &sound) const
{
    alSourcei(source_id, AL_BUFFER, sound.buffer_id);
    alSourcePlay(source_id);
}

void liminal::audio_source::resume() const
{
    alSourcePlay(source_id);
}

void liminal::audio_source::pause() const
{
    alSourcePause(source_id);
}

void liminal::audio_source::stop() const
{
    alSourceStop(source_id);
}
