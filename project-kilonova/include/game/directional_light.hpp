#ifndef GAME_DIRECTIONAL_LIGHT_HPP
#define GAME_DIRECTIONAL_LIGHT_HPP

namespace pk
{
    struct directional_light
    {
        vec3 direction;
        vec3 ambient_color;
        vec3 diffuse_color;
        vec3 specular_color;

        directional_light(
            vec3 direction,
            vec3 ambient_color,
            vec3 diffuse_color,
            vec3 specular_color);
        ~directional_light();
    };
}

#endif
