#ifndef ENGINE_H
#define ENGINE_H

#include <cglm/cglm.h>
#include <float.h>
#include <GL/glew.h>
#include <malloc.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_ttf.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audio.h"
#include "bitmap.h"
#include "camera.h"
#include "directional_light.h"
#include "error.h"
#include "material.h"
#include "mesh.h"
#include "net.h"
#include "object.h"
#include "point_light.h"
#include "program.h"
#include "spot_light.h"
#include "texture.h"
#include "time.h"
#include "window_gl.h"
#include "window_sw.h"

int engine_init(void);
void engine_quit(void);

#endif
