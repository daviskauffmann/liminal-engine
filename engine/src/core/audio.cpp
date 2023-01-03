#include <liminal/core/audio.hpp>

#include <stdexcept>

liminal::audio::audio(
    const int frequency,
    const std::uint16_t format,
    const int channels,
    const int chunk_size)
{
    if (Mix_OpenAudio(frequency, format, channels, chunk_size) != 0)
    {
        throw std::runtime_error(Mix_GetError());
    }
}

liminal::audio::~audio()
{
    Mix_CloseAudio();
}
