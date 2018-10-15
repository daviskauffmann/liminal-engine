#ifndef CONFIG_H
#define CONFIG_H

#define WINDOW_TITLE "Game Engine v0.1"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FPS_CAP 60
#define FRAME_DELAY (1000 / FPS_CAP)

extern int window_width;
extern int window_height;

int config_load(void);
void config_save(void);

#endif
