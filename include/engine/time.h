#ifndef TIME_H
#define TIME_H

void ENGINE_API time_cap_fps(unsigned int fps_cap);
void ENGINE_API time_frame_start(void);
void ENGINE_API time_frame_end(void);
unsigned int ENGINE_API time_current(void);
float ENGINE_API time_delta(void);
unsigned int ENGINE_API time_fps(void);

#endif
