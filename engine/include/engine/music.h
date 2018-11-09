#ifndef MUSIC_H
#define MUSIC_H

struct music
{
    Mix_Music *music;
};

struct music ENGINE_API *music_create(const char *file);
int ENGINE_API music_play(struct music *music, int loops);
void ENGINE_API music_destroy(struct music *music);

#endif
