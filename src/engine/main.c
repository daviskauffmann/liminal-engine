#include <GL/glew.h>
#include <SDL/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#include "../game/game.h"
#include "config.h"
#include "time.h"
#include "window.h"

int main(int argc, char *argv[])
{
    int error = 0;

    if (error = config_load())
    {
        printf("Error: %s\n", SDL_GetError());

        return error;
    }

    if (error = window_init(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        printf("Error: %s\n", SDL_GetError());

        return error;
    }

    if (error = game_init())
    {
        printf("Error: %s\n", SDL_GetError());

        return error;
    }

    bool quit = false;

    while (!quit)
    {
        unsigned int frame_start = SDL_GetTicks();

        time_update();

        quit = game_input();

        game_update();

        window_clear();

        game_render();

        window_render();

        unsigned int frame_end = SDL_GetTicks();
        unsigned int frame_time = frame_end - frame_start;

        if (FRAME_DELAY > frame_time)
        {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    }

    game_quit();
    window_quit();
    config_save();

    return 0;
}
