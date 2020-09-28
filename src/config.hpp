#ifndef CONFIG_HPP
#define CONFIG_HPP

constexpr const char *window_title = "Project Kilonova";
constexpr const char *version = "0.0.1";

extern int window_width;
extern int window_height;
extern float render_scale;

void config_load();
void config_save();

#endif
