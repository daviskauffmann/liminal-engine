#include <liminal/core/al_context.hpp>

#include <stdexcept>

liminal::core::al_context::al_context(ALCdevice *const alc_device)
{
    alc_context = alcCreateContext(alc_device, nullptr);
    if (!alc_context)
    {
        throw std::runtime_error("Failed to create OpenAL context");
    }

    if (!alcMakeContextCurrent(alc_context))
    {
        alcDestroyContext(alc_context);

        throw std::runtime_error("Failed to make OpenAL context current");
    }
}

liminal::core::al_context::~al_context()
{
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(alc_context);
}
