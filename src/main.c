#include <GL/glew.h>
#include <SDL/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#include "config.h"
#include "game.h"
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
    unsigned int current_time = 0;

    while (!quit)
    {
        unsigned int frame_start = SDL_GetTicks();

        unsigned int previous_time = current_time;
        current_time = frame_start;
        float delta_time = (current_time - previous_time) / 1000.0f;

        static float fps_update_timer = 0.0f;
        static unsigned int fps = 0;
        fps_update_timer += delta_time;
        if (fps_update_timer >= 0.25f)
        {
            fps_update_timer = 0.0f;
            fps = (unsigned int)(1 / delta_time);
        }

        int num_keys;
        const unsigned char *keys = SDL_GetKeyboardState(&num_keys);

        int mouse_x, mouse_y;
        unsigned int mouse = SDL_GetMouseState(&mouse_x, &mouse_y);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            quit = game_input(num_keys, keys, mouse_x, mouse_y, mouse, event);
        }

        game_update(delta_time);

        window_clear();

        game_render();

        window_render();

        char title[256];
        sprintf(title, "%s - FPS: %d", WINDOW_TITLE, fps);
        window_set_title(title);

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
