#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include <stdbool.h>

int game_init(void);
bool game_input(
    int num_keys,
    const unsigned char *keys,
    int mouse_x,
    int mouse_y,
    unsigned int mouse,
    SDL_Event event);
void game_update(float delta_time);
void game_render(void);
void game_quit(void);

#endif
