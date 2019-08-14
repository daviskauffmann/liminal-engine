#ifndef GAME_POINT_LIGHT_HPP
#define GAME_POINT_LIGHT_HPP

namespace pk
{
    struct point_light
    {
        vec3 position;
        vec3 ambient_color;
        vec3 diffuse_color;
        vec3 specular_color;
        vec3 attenuation;

        point_light(
            vec3 position,
            vec3 ambient_color,
            vec3 diffuse_color,
            vec3 specular_color,
            vec3 attenuation);
        ~point_light();
    };
}

#endif
