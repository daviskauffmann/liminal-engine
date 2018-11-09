#ifndef AUDIO_H
#define AUDIO_H

int ENGINE_API audio_init(int frequency, unsigned short format, int channels, int chunk_size);
bool ENGINE_API audio_playing_music(void);
bool ENGINE_API audio_paused_music(void);
void ENGINE_API audio_pause_music(void);
void ENGINE_API audio_resume_music(void);
void ENGINE_API audio_stop_music(void);
void ENGINE_API audio_quit(void);

#endif
