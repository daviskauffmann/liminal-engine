#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include <stdbool.h>

int game_init(void);
bool game_input(float delta_time);
void game_update(unsigned int current_time, float delta_time);
void game_render(void);
void game_quit(void);

#endif
