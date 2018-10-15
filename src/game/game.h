#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include <stdbool.h>

int game_init(void);
bool game_input(void);
void game_update(void);
void game_render(void);
void game_quit(void);

#endif
