#include <engine/engine.h>

static ALCdevice *device;
static ALCcontext *context;

int audio_init(void)
{
    // setup OpenAL
    device = alcOpenDevice(NULL);

    if (!device)
    {
        return 1;
    }

    context = alcCreateContext(device, NULL);

    if (!context)
    {
        return 1;
    }

    if (!alcMakeContextCurrent(context))
    {
        return 1;
    }

    return 0;
}

void audio_set_listener(vec3 position, vec3 velocity, vec3 *orientation)
{
    alListenerfv(AL_POSITION, (ALfloat *)position);
    alListenerfv(AL_VELOCITY, (ALfloat *)velocity);
    alListenerfv(AL_ORIENTATION, (ALfloat *)orientation);
}

void audio_quit(void)
{
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}
