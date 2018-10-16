#ifndef TIME_H
#define TIME_H

void time_cap_fps(unsigned int fps_cap);
void time_frame_start(void);
void time_frame_end(void);
unsigned int time_current(void);
float time_delta(void);
unsigned int time_fps(void);

#endif
