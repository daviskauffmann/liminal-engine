#include <engine/engine.h>

struct music ENGINE_API *music_create(const char *file)
{
    struct music *music = malloc(sizeof(struct music));

    if (!music)
    {
        error("Couldn't allocate music");

        return NULL;
    }

    music->music = Mix_LoadMUS(file);

    if (!music->music)
    {
        error(Mix_GetError());

        return NULL;
    }

    return music;
}

int ENGINE_API music_play(struct music *music, int loops)
{
    return Mix_PlayMusic(music->music, loops);
}

void ENGINE_API music_destroy(struct music *music)
{
    Mix_FreeMusic(music->music);
    free(music);
}
