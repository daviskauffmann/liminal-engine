#ifndef ENGINE_H
#define ENGINE_H

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
#include "window.h"

int engine_init(void);
void engine_quit(void);

#endif
