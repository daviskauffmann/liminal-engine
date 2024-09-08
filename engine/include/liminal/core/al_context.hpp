#ifndef LIMINAL_CORE_AL_CONTEXT_HPP
#define LIMINAL_CORE_AL_CONTEXT_HPP

#include <AL/alc.h>

namespace liminal
{
    namespace core
    {
        class al_context
        {
        public:
            al_context(ALCdevice *alc_device);
            ~al_context();
            al_context(const al_context &other) = delete;
            al_context &operator=(const al_context &other) = delete;
            al_context(al_context &&other) = delete;
            al_context &operator=(al_context &&other) = delete;

        private:
            ALCcontext *alc_context;
        };
    }
}

#endif
