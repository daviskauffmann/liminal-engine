#include <liminal/core/al_device.hpp>

#include <stdexcept>

liminal::core::al_device::al_device()
{
    alc_device = alcOpenDevice(nullptr);
    if (!alc_device)
    {
        throw std::runtime_error("Failed to open OpenAL device");
    }
}

liminal::core::al_device::~al_device()
{
    alcCloseDevice(alc_device);
}

ALCdevice *liminal::core::al_device::get_alc_device() const
{
    return alc_device;
}
