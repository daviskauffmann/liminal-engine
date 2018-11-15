#include <engine/engine.h>

static ALCdevice *device;
static ALCcontext *context;

// TODO: handle OpenAL errors

int audio_init(int frequency, unsigned short format, int channels, int chunk_size)
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

    // setup SDL_mixer
    if (Mix_OpenAudio(frequency, format, channels, chunk_size))
    {
        error(Mix_GetError());

        return 1;
    }

    return 0;
}

void audio_set_listener(vec3 position, vec3 velocity, vec3 orientation)
{
    alListener3f(AL_POSITION, position[0], position[1], position[2]);
    alListener3f(AL_VELOCITY, velocity[0], velocity[1], velocity[2]);
    alListener3f(AL_ORIENTATION, orientation[0], orientation[1], orientation[2]);
}

void audio_quit(void)
{
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
    Mix_CloseAudio();
}
