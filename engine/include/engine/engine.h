#ifndef ENGINE_H
#define ENGINE_H

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include <cglm/cglm.h>
#include <GL/glew.h>
#include <malloc.h>
#include <math.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_net.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audio.h"
#include "bitmap.h"
#include "camera.h"
#include "chunk.h"
#include "core.h"
#include "cubemap.h"
#include "directional_light.h"
#include "http.h"
#include "log.h"
#include "material.h"
#include "mesh.h"
#include "object.h"
#include "point_light.h"
#include "program.h"
#include "renderer.h"
#include "scene.h"
#include "spot_light.h"
#include "sprite.h"
#include "tcp.h"
#include "texture.h"
#include "time.h"
#include "udp.h"
#include "water.h"
#include "window.h"

int ENGINE_API engine_init(void);
void ENGINE_API engine_quit(void);

#endif
