#ifndef WINDOW_H
#define WINDOW_H

int window_init(const char *title, int width, int height);
const char *window_get_title(void);
void window_set_title(const char *title);
void window_toggle_fullscreen(void);
void window_resize(int width, int height);
void window_clear(void);
void window_render(void);
void window_quit(void);

#endif
