#ifndef LIMINAL_CORE_AL_DEVICE_HPP
#define LIMINAL_CORE_AL_DEVICE_HPP

#include <AL/alc.h>

namespace liminal
{
    namespace core
    {
        class al_device
        {
        public:
            al_device();
            ~al_device();
            al_device(const al_device &other) = delete;
            al_device &operator=(const al_device &other) = delete;
            al_device(al_device &&other) = delete;
            al_device &operator=(al_device &&other) = delete;

            ALCdevice *get_alc_device() const;

        private:
            ALCdevice *alc_device;
        };
    }
}

#endif
