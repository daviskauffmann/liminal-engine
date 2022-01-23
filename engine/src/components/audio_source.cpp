#include <liminal/components/audio_source.hpp>

liminal::audio_source::audio_source()
{
    source = new liminal::source();
}

liminal::audio_source::~audio_source()
{
    delete source;
}
