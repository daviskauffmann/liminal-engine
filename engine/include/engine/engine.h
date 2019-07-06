#ifndef ENGINE_H
#define ENGINE_H

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include <malloc.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <cglm/cglm.h>
#include <GL/glew.h>
#include <stb_image.h>

#include "audio/audio.h"
#include "audio/sound.h"
#include "audio/source.h"
#include "entities/camera.h"
#include "entities/directional_light.h"
#include "entities/object.h"
#include "entities/point_light.h"
#include "entities/spot_light.h"
#include "entities/sprite.h"
#include "entities/sun.h"
#include "entities/water.h"
#include "rendering/assets.h"
#include "rendering/cubemap.h"
#include "rendering/material.h"
#include "rendering/mesh.h"
#include "rendering/program.h"
#include "rendering/renderer.h"
#include "rendering/texture.h"

// Initializes the engine. This function assumes an OpenGL context has already been created.
int ENGINE_API engine_init(void);

#endif
