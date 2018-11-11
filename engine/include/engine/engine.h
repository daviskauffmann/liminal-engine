#ifndef ENGINE_H
#define ENGINE_H

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include <AL/al.h>
#include <AL/alc.h>
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

#include "audio/audio.h"
#include "audio/sound.h"
#include "audio/source.h"
#include "core/bitmap.h"
#include "core/core.h"
#include "core/log.h"
#include "core/time.h"
#include "entities/camera.h"
#include "entities/directional_light.h"
#include "entities/object.h"
#include "entities/point_light.h"
#include "entities/spot_light.h"
#include "entities/sprite.h"
#include "entities/water.h"
#include "net/http.h"
#include "net/tcp.h"
#include "net/udp.h"
#include "rendering/cubemap.h"
#include "rendering/material.h"
#include "rendering/mesh.h"
#include "rendering/program.h"
#include "rendering/renderer.h"
#include "rendering/texture.h"
#include "rendering/window.h"

#endif
