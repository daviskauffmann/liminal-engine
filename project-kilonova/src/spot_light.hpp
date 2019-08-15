#ifndef GAME_SPOT_LIGHT_HPP
#define GAME_SPOT_LIGHT_HPP

#include <cglm/cglm.h>

namespace pk
{
    struct spot_light
    {
        vec3 position;
        vec3 direction;
        vec3 ambient_color;
        vec3 diffuse_color;
        vec3 specular_color;
        vec3 attenuation;
        float inner_cutoff;
        float outer_cutoff;

        spot_light(
            vec3 position,
            vec3 direction,
            vec3 ambient_color,
            vec3 diffuse_color,
            vec3 specular_color,
            vec3 attenuation,
            float inner_cutoff,
            float outer_cutoff);
        ~spot_light();
    };
}

#endif
